/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_COLLISION
#define UNGINE_COLLISION

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace collision {

    box_t box( mesh_t  mesh  ){ return rl::GetMeshBoundingBox ( mesh  ); }
    box_t box( model_t model ){ return rl::GetModelBoundingBox( model ); }

    box_t get_hit( box_t A, box_t B ){
        if( !rl::CheckCollisionBoxes( A, B ) ){ return box_t({0}); }

        float overlap_x = fminf( A.max.x, B.max.x )- 
                          fmaxf( A.min.x, B.min.x );
    
        float overlap_z = fminf( A.max.z, B.max.z )-
                          fmaxf( A.min.z, B.min.z );

        box_t  push = { 0 };
        vec3_t pa   = ( A.max + A.min ) / 2;
        vec3_t pb   = ( B.max + B.min ) / 2;

        if( overlap_x < overlap_z ){
            push.min.x = ( pa.x > (B.min.x + B.max.x)/2 ? overlap_x : -overlap_x);
        } else {
            push.min.z = ( pa.z > (B.min.z + B.max.z)/2 ? overlap_z : -overlap_z);
        }

        return push;
    }

    bool is_hit( box_t A, box_t B ){ return rl::CheckCollisionBoxes( A, B ); }

} }

namespace ungine { namespace collision {
    
    ray_hit_t get_hit( ray_t ray, mesh_t mesh, transform_3D_t transform ){
        rl::Matrix mat = math::matrix::from_transform_3D( transform );
        return rl::GetRayCollisionMesh( ray, mesh, mat );
    }

    ray_hit_t get_hit( ray_t ray, box_t box ){
        return rl::GetRayCollisionBox( ray, box );
    }

} }

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace collision { 
    
    bool is_overlaped( ptr_t<float> proj_a, ptr_t<float> proj_b ){
        if( proj_a.empty() || proj_b.empty() ){ return false; }
        return proj_a[0]<=proj_b[1] && proj_b[0]<=proj_a[1];
    }
    
    /*─······································································─*/

    ptr_t<vec3_t> get_3D_collision_axes( node_t a ){ do {
        auto pos = a.get_attribute<transform_3D_t>( "transform" );
        if ( pos == nullptr ){ break; }

        auto rot = math::matrix::rotation( math::negate( pos->translate.rotation ) );
        auto out = ptr_t<vec3_t>( 3 );

        out[0] = vec3_t({ rot.m0, rot.m4, rot.m8  });
        out[1] = vec3_t({ rot.m1, rot.m5, rot.m9  });
        out[2] = vec3_t({ rot.m2, rot.m6, rot.m10 });

    return out; } while(0); return nullptr; }
    
    /*─······································································─*/

    ptr_t  <float> get_3D_collision_projection( node_t a, vec3_t axis ){ do {

        auto pos  = a.get_attribute<transform_3D_t>( "transform" );
        auto col  = a.get_attribute<collision_t>   ( "collision" );
        auto shp  = a.get_attribute<shape_3D_t>    ( "shape" );

    if( pos==nullptr || col==nullptr || shp == nullptr  ){ break; }
    if( col->mode & collision::MODE::COLLISION_MODE_RAY ){

        auto rot   = math::matrix::rotation( math::negate( pos->translate.rotation ) );
        auto fr    = vec3_t({ rot.m2, rot.m6, rot.m10 }); 

        auto start = /*--------*/ pos->translate.position;
        auto stop  = start + fr * pos->translate.scale;

        float p_start = Vector3DotProduct( start, axis );
        float p_stop  = Vector3DotProduct( stop,  axis );

        float rmin = fminf( p_start, p_stop );
        float rmax = fmaxf( p_start, p_stop );
        
        return ptr_t<float>({ 
            rmin, /*----------*/ rmax, 
            pos->translate.position.x,
            pos->translate.position.y,
            pos->translate.position.z
        });

    } else {

        mat_t transform = rl::MatrixCompose( 
            pos->translate.position, 
        rl::QuaternionFromEuler( 
            pos->rotation.x, pos->rotation.y, 
            /*------------*/ pos->rotation.z
        ),  pos->translate.scale );
        
        queue_t<float> out;

        for( int y=0; y<shp->model.meshCount; y++ ){
            
            float rmin=FLT_MAX; float rmax=-FLT_MAX;
            rl::Mesh mesh = shp->model.meshes[y];
            int stride    = mesh.vertices? 3 : 0;
            if( stride == 0 ) /*-*/ { continue; }

            vec3_t mean = { 0., 0., 0. };
            
        for( int x=0; x<mesh.vertexCount; x++ ){
             int baseIndex = x * stride;        
        if ( baseIndex + 2 >= mesh.vertexCount * stride ){ break; }
                
            vec3_t vertex = {
                mesh.vertices[baseIndex + 0],
                mesh.vertices[baseIndex + 1], 
                mesh.vertices[baseIndex + 2]
            };
        
            vec3_t world_point = Vector3Transform (vertex, transform);
            float /*------*/ p = Vector3DotProduct(world_point, axis);
                
            rmin = fminf(rmin, p); rmax = fmaxf(rmax, p);
            mean.x += world_point.x * 3 / mesh.vertexCount;
            mean.y += world_point.y * 3 / mesh.vertexCount;
            mean.z += world_point.z * 3 / mesh.vertexCount;

        }   out.insert( 0, {
                rmin, /*-------*/ rmax, 
                mean.x, mean.y, mean.z
        }); }  
        
    return out.data(); }} while(0); return nullptr; }
    
    /*─······································································─*/

    float get_3D_overlap_depth( node_t a, node_t b, vec3_t axis, float* sign ){

        auto proj_a = get_3D_collision_projection(a, axis);
        auto proj_b = get_3D_collision_projection(b, axis);

    if ( proj_a.null() || proj_b.null() ){ return .0f; }
    for( int x=0; x<proj_a.size(); x+=5 ){
    for( int y=0; y<proj_b.size(); y+=5 ){

        auto   obj_a= proj_a; obj_a.slice( x, x+2 );
        auto   obj_b= proj_b; obj_b.slice( y, y+2 );
        vec3_t org_a, org_b;

        memcpy( &org_a, proj_a.get() + 2, sizeof( vec3_t ) );
        memcpy( &org_b, proj_b.get() + 2, sizeof( vec3_t ) );

    if ( !is_overlaped( obj_a, obj_b ) ){ continue; }
        
        float overlap1 = proj_a[1] - proj_b[0];
        float overlap2 = proj_b[1] - proj_a[0];
        float depth    = fmin( overlap1, overlap2 );

        float center_proj_a = Vector3DotProduct( org_a, axis );
        float center_proj_b = Vector3DotProduct( org_b, axis );
        
        if( depth == overlap1 ) {
                 *sign = center_proj_a > center_proj_b ? 1.0f :-1.0f;
        } else { *sign = center_proj_b > center_proj_a ?-1.0f : 1.0f; }
        
        return depth;
    }}  return .0f  ; }
    
    /*─······································································─*/

    ptr_t<overlap_3D_t> get_3D_collision( node_t a, node_t b ){ overlap_3D_t sign; do {

        auto axes_a = get_3D_collision_axes(a); if( axes_a.empty() ){ break; }
        auto axes_b = get_3D_collision_axes(b); if( axes_b.empty() ){ break; }

        for( auto& axis : axes_a ){ float n_sign = .0f;

            float overlap = get_3D_overlap_depth( a, b, axis, &n_sign ); 

            if( overlap < EPSILON      ){ return nullptr; }
            if( overlap < sign.overlap ){
                sign.overlap = overlap;
                sign.axis    = axis   ;
                sign.sign    = n_sign ;
            }

        }
        
        for( auto& axis : axes_b ){ float n_sign = .0f;

            float overlap = get_3D_overlap_depth( a, b, axis, &n_sign ); 

            if( overlap < EPSILON      ){ return nullptr; }
            if( overlap < sign.overlap ){
                sign.overlap = overlap;
                sign.axis    = axis   ;
                sign.sign    = n_sign ;
            }

        }

        for( int i=0; i<3; ++i ){ for( int j=0; j<3; ++j ){

            vec3_t axis = Vector3CrossProduct( axes_a[i], axes_b[j] );

            if( Vector3Length(axis) < EPSILON ){ continue; }

            float n_sign  = .0f;
            float overlap = get_3D_overlap_depth( a, b, axis, &n_sign ); 

            if( overlap < EPSILON      ){ return nullptr; }
            if( overlap < sign.overlap ){
                sign.overlap = overlap;
                sign.axis    = axis   ;
                sign.sign    = n_sign ;
            }
            
        }}

    /*--*/ sign.point = Vector3Scale( sign.axis, sign.overlap * sign.sign );
    return type::bind( sign ); } while(0); return nullptr; }
    
    /*─······································································─*/

    bool get_3D_collision_weak( node_t a, node_t b ) {
        auto box_a = a.get_attribute<shape_3D_t>( "shape" );
        auto box_b = b.get_attribute<shape_3D_t>( "shape" );
        return rl::CheckCollisionBoxes( box_a->bound, box_b->bound );
    }
    
}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace collision { 

    ptr_t<vec2_t> get_2D_collision_axes( node_t a ){ do {

        auto pos = a.get_attribute<transform_2D_t>( "transform" );
        if ( pos == nullptr ){ break; } 

        auto rot = pos->translate.rotation;
        
        return ptr_t<vec2_t>({
            vec2_t({  cosf(rot), sinf(rot) }),
            vec2_t({ -sinf(rot), cosf(rot) })
        });

    } while(0); return nullptr; }
    
    /*─······································································─*/

    ptr_t<float> get_2D_collision_projection( node_t a, vec2_t axis ){ do {

        auto pos  = a.get_attribute<transform_2D_t>( "transform" );
        auto col  = a.get_attribute<collision_t>   ( "collision" );
        auto shp  = a.get_attribute<shape_2D_t>    ( "shape" );

    if( pos==nullptr || col==nullptr || shp == nullptr ){ break; }

        queue_t<float> out;

        for( int y=0; y<shp->model.meshCount; y++ ){
            
            float rmin=FLT_MAX; float rmax=-FLT_MAX;
            rl::Mesh mesh = shp->model.meshes[y];
            int stride    = mesh.vertices? 3 : 0;
            if( stride == 0 ){ continue; }

            vec2_t mean = { 0., 0. };
            
        for( int x=0; x<mesh.vertexCount; x++ ){
             int baseIndex = x * stride;
        if ( baseIndex + 2 >= mesh.vertexCount * stride ){ break; }
                
            vec2_t vertex = {
                mesh.vertices[baseIndex + 0],
                mesh.vertices[baseIndex + 1]
            //  mesh.vertices[baseIndex + 2]
            };

            vec2_t scaled_point  = Vector2Multiply( vertex       , pos->translate.scale    );
            vec2_t rotated_point = Vector2Rotate  ( scaled_point , pos->translate.rotation );
            vec2_t world_point   = Vector2Add     ( rotated_point, pos->translate.position );
                
            float p = Vector2DotProduct( world_point, axis );
                
            rmin = fminf(rmin, p); rmax = fmaxf(rmax, p);
            mean.x += world_point.x * 3 / mesh.vertexCount;
            mean.y += world_point.y * 3 / mesh.vertexCount;

        }   out.insert( 0, {
                rmin  , rmax  , 
                mean.x, mean.y
        }); }
        
        return out.data();
    }   while(0); return nullptr; }

    /*─······································································─*/

    float get_2D_overlap_depth( node_t a, node_t b, vec2_t axis, float* sign ){

        auto proj_a = get_2D_collision_projection(a, axis);
        auto proj_b = get_2D_collision_projection(b, axis);

    if ( proj_a.null() || proj_b.null() ){ return .0f; }
    for( int x=0; x<proj_a.size(); x+=5 ){
    for( int y=0; y<proj_b.size(); y+=5 ){

        auto   obj_a= proj_a; obj_a.slice( x, x+1 );
        auto   obj_b= proj_b; obj_b.slice( y, y+1 );
        vec2_t org_a, org_b;

        memcpy( &org_a, proj_a.get() + 1, sizeof( vec2_t ) );
        memcpy( &org_b, proj_b.get() + 1, sizeof( vec2_t ) );

    if ( !is_overlaped( obj_a, obj_b ) ){ continue; }
        
        float overlap1 = proj_a[1] - proj_b[0];
        float overlap2 = proj_b[1] - proj_a[0];

        float depth = fmin( overlap1, overlap2 );

        float center_proj_a = Vector2DotProduct( org_a, axis );
        float center_proj_b = Vector2DotProduct( org_b, axis );
        
        if( depth == overlap1 ) {
                 *sign = center_proj_a > center_proj_b ? 1.0f :-1.0f;
        } else { *sign = center_proj_b > center_proj_a ?-1.0f : 1.0f; }
        
        return depth;
    }}  return .0f  ; }
    
    /*─······································································─*/

    ptr_t<overlap_2D_t> get_2D_collision( node_t a, node_t b ){ overlap_2D_t sign; do {
    
        auto axes_a = get_2D_collision_axes(a); if( axes_a.empty() ){ break; }
        auto axes_b = get_2D_collision_axes(b); if( axes_b.empty() ){ break; }

        for( auto& axis : axes_a ){ float n_sign = .0f;

            float overlap = get_2D_overlap_depth( a, b, axis, &n_sign ); 

            if( overlap < EPSILON      ){ return nullptr; }
            if( overlap < sign.overlap ){
                sign.overlap = overlap;
                sign.axis    = axis   ;
                sign.sign    = n_sign ;
            }

        }

        for( auto& axis : axes_b ){ float n_sign = .0f;

            float overlap = get_2D_overlap_depth( a, b, axis, &n_sign ); 

            if( overlap < EPSILON      ){ return nullptr; }
            if( overlap < sign.overlap ){
                sign.overlap = overlap;
                sign.axis    = axis   ;
                sign.sign    = n_sign ;
            }

        }

    /*--*/ sign.point = Vector2Scale( sign.axis, sign.overlap * sign.sign );
    return type::bind( sign ); } while(0); return nullptr; }
    
    /*─······································································─*/

    bool get_2D_collision_weak( node_t a, node_t b ) {
        auto box_a = a.get_attribute<shape_2D_t>( "shape" );
        auto box_b = b.get_attribute<shape_2D_t>( "shape" );
        return rl::CheckCollisionBoxes( box_a->bound, box_b->bound );
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace collision { void check_collision( node_t a, node_t b ){

    auto mode = collision::MODE::COLLISION_MODE_2D | collision::MODE::COLLISION_MODE_3D;

    auto vis1 = a.get_attribute<visibility_t>( "visibility" );
    auto vis2 = b.get_attribute<visibility_t>( "visibility" );
    auto col1 = a.get_attribute<collision_t> ( "collision" );
    auto col2 = b.get_attribute<collision_t> ( "collision" );

    if(( col1.null() || col2.null() )) /*-*/ { return; }
    if(( col1->mode & col2->mode & mode )==0){ return; }
    if(( col1->mask & col2->mask /*--*/ )==0){ return; }

    if(  vis1->mode == 0x00 ) /*---*/ { return; }
    if(  vis2->mode == 0x00 ) /*---*/ { return; }
    if(( vis1->mask & vis2->mask )==0){ return; }

    if( col1->mode & collision::MODE::COLLISION_MODE_2D ){
    if( !get_2D_collision_weak( a, b ) ){ return; }

        auto overlap = get_2D_collision( a, b );
        if( overlap.null() ) /*----*/ { return; }
        if( !b.exists()||!a.exists() ){ return; }

    } else {
    if( !get_3D_collision_weak( a, b ) ){ return; }
        
        auto overlap = get_3D_collision( a, b );
        if( overlap.null() ) /*----*/ { return; }
        if( !b.exists()||!a.exists() ){ return; }
        
    }

}}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace node { void check_collision( void* x, queue_t<void*> queue ){

    if( x==nullptr ){ return; } queue.map([=]( void* y ){

        if( x==y ) /*------------*/ { return; }
        node_t *X= type::cast<node_t>(x), 
               *Y= type::cast<node_t>(y);

        collision::check_collision( *X,*Y );

    });

}}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace node {

    node_t node_collision_model( const model_t& model, function_t<void,ptr_t<node_t>> clb ){
    return node_model( model, [=]( ptr_t<node_t> self ){

        auto tmp /**/ = collision_t();
             tmp.mode = collision::MODE::COLLISION_MODE_3D  |
                        collision::MODE::COLLISION_MODE_MESH;
             tmp.mask = collision::MASK::COLLISION_MASK_ALL ;
        
        self->set_attribute( "collision", tmp ); 

        auto pos = self->get_attribute<transform_3D_t>( "transform" );
        auto col = self->get_attribute<collision_t>   ( "collision" );
        auto shp = self->get_attribute<shape_3D_t>    ( "shape" );
        shp->mode= shape::MODE::SHAPE_MODE_NONE;

        auto vpt= self->get_root_viewport(); if( vpt!=nullptr ){
             vpt->queue_collision.push( &self );

        auto ID = vpt->queue_collision.last(); self->onClose([=](){
             vpt->queue_collision.erase( ID );
        }); }

        self->onNext([=](){ if( col->mode==0x00 )/**/{ return; }
            node::check_collision( &self, vpt->queue_collision ); 
        });

    clb( self ); }); }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace node {

    node_t node_collision_line( function_t<void,ptr_t<node_t>> clb ){
    return node_line([=]( ptr_t<node_t> self ){

        auto tmp /**/ = collision_t();
             tmp.mode = collision::MODE::COLLISION_MODE_2D |
                        collision::MODE::COLLISION_MODE_RAY;
             tmp.mask = collision::MASK::COLLISION_MASK_ALL;
        
        self->set_attribute( "collision", tmp ); 

        auto pos = self->get_attribute<transform_2D_t>( "transform" );
        auto col = self->get_attribute<collision_t>   ( "collision" );
        auto shp = self->get_attribute<shape_2D_t>    ( "shape" );
        shp->mode= shape::MODE::SHAPE_MODE_NONE;

        auto vpt= self->get_root_viewport(); if( vpt!=nullptr ){
             vpt->queue_collision.push( &self );

        auto ID = vpt->queue_collision.last(); self->onClose([=](){
            vpt->queue_collision.erase( ID );
        }); }

        self->onNext([=](){ if( col->mode==0x00 )/**/{ return; }
            node::check_collision( &self, vpt->queue_collision ); 
        });

    clb( self ); }); }

    node_t node_collision_rectangle( function_t<void,ptr_t<node_t>> clb ){
    return node_rectangle([=]( ptr_t<node_t> self ){

        auto tmp /**/ = collision_t();
             tmp.mode = collision::MODE::COLLISION_MODE_2D |
                        collision::MODE::COLLISION_MODE_BOX;
             tmp.mask = collision::MASK::COLLISION_MASK_ALL;
        
        self->set_attribute( "collision", tmp ); 

        auto pos = self->get_attribute<transform_2D_t>( "transform" );
        auto col = self->get_attribute<collision_t>   ( "collision" );
        auto shp = self->get_attribute<shape_2D_t>    ( "shape" );
        shp->mode= shape::MODE::SHAPE_MODE_NONE;

        auto vpt= self->get_root_viewport(); if( vpt!=nullptr ){
             vpt->queue_collision.push( &self );

        auto ID = vpt->queue_collision.last(); self->onClose([=](){
            vpt->queue_collision.erase( ID );
        }); }

        self->onNext([=](){ if( col->mode==0x00 )/**/{ return; }
            node::check_collision( &self, vpt->queue_collision ); 
        });

    clb( self ); }); }

    node_t node_collision_circle( function_t<void,ptr_t<node_t>> clb ){
    return node_circle([=]( ptr_t<node_t> self ){

        auto tmp /**/ = collision_t();
             tmp.mode = collision::MODE::COLLISION_MODE_2D |
                        collision::MODE::COLLISION_MODE_SPH;
             tmp.mask = collision::MASK::COLLISION_MASK_ALL;
        
        self->set_attribute( "collision", tmp );

        auto pos = self->get_attribute<transform_2D_t>( "transform" );
        auto col = self->get_attribute<collision_t>   ( "collision" );
        auto shp = self->get_attribute<shape_2D_t>    ( "shape" );
        shp->mode= shape::MODE::SHAPE_MODE_NONE;

        auto vpt= self->get_root_viewport(); if( vpt!=nullptr ){
             vpt->queue_collision.push( &self );

        auto ID = vpt->queue_collision.last(); self->onClose([=](){
             vpt->queue_collision.erase( ID );
        }); }

        self->onNext([=](){ if( col->mode==0x00 )/**/{ return; }
            node::check_collision( &self, vpt->queue_collision ); 
        });

    clb( self ); }); }
    
    /*─······································································─*/

    node_t node_collision_ray( function_t<void,ptr_t<node_t>> clb ){
    return node_ray([=]( ptr_t<node_t> self ){

        auto tmp /**/ = collision_t();
             tmp.mode = collision::MODE::COLLISION_MODE_3D |
                        collision::MODE::COLLISION_MODE_RAY;
             tmp.mask = collision::MASK::COLLISION_MASK_ALL;
        
        self->set_attribute( "collision", tmp ); 

        auto pos = self->get_attribute<transform_3D_t>( "transform" );
        auto col = self->get_attribute<collision_t>   ( "collision" );
        auto shp = self->get_attribute<shape_3D_t>    ( "shape" );
        shp->mode= shape::MODE::SHAPE_MODE_NONE;

        auto vpt= self->get_root_viewport(); if( vpt!=nullptr ){
             vpt->queue_collision.push( &self );

        auto ID = vpt->queue_collision.last(); self->onClose([=](){
            vpt->queue_collision.erase( ID );
        }); }

        self->onNext([=](){ if( col->mode==0x00 )/**/{ return; }
            node::check_collision( &self, vpt->queue_collision ); 
        });

    clb( self ); }); }

    node_t node_collision_cube( function_t<void,ptr_t<node_t>> clb ){
    return node_cube([=]( ptr_t<node_t> self ){

        auto tmp /**/ = collision_t();
             tmp.mode = collision::MODE::COLLISION_MODE_3D |
                        collision::MODE::COLLISION_MODE_BOX;
             tmp.mask = collision::MASK::COLLISION_MASK_ALL;
        
        self->set_attribute( "collision", tmp ); 

        auto pos = self->get_attribute<transform_3D_t>( "transform" );
        auto col = self->get_attribute<collision_t>   ( "collision" );
        auto shp = self->get_attribute<shape_3D_t>    ( "shape" );
        shp->mode= shape::MODE::SHAPE_MODE_NONE;

        auto vpt= self->get_root_viewport(); if( vpt!=nullptr ){
             vpt->queue_collision.push( &self );

        auto ID = vpt->queue_collision.last(); self->onClose([=](){
             vpt->queue_collision.erase( ID );
        }); }

        self->onNext([=](){ if( col->mode==0x00 )/**/{ return; }
            node::check_collision( &self, vpt->queue_collision ); 
        });

    clb( self ); }); }

    node_t node_collision_sphere( function_t<void,ptr_t<node_t>> clb ){
    return node_sphere([=]( ptr_t<node_t> self ){

        auto tmp /**/ = collision_t();
             tmp.mode = collision::MODE::COLLISION_MODE_3D |
                        collision::MODE::COLLISION_MODE_SPH;
             tmp.mask = collision::MASK::COLLISION_MASK_ALL;
        
        self->set_attribute( "collision", tmp );

        auto pos = self->get_attribute<transform_3D_t>( "transform" );
        auto col = self->get_attribute<collision_t>   ( "collision" );
        auto shp = self->get_attribute<shape_3D_t>    ( "shape" );
        shp->mode= shape::MODE::SHAPE_MODE_NONE;

        auto vpt= self->get_root_viewport(); if( vpt!=nullptr ){
             vpt->queue_collision.push( &self );

        auto ID = vpt->queue_collision.last(); self->onClose([=](){
             vpt->queue_collision.erase( ID );
        }); }

        self->onNext([=](){ if( col->mode==0x00 )/**/{ return; }
            node::check_collision( &self, vpt->queue_collision ); 
        });
        
    clb( self ); }); }
    
}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/
