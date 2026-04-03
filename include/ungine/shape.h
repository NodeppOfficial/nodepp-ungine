/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_SHAPES
#define UNGINE_SHAPES

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace node {

    node_t node_polygon( const model_t& model, function_t<void,ptr_t<node_t>> clb ) {
    return node_2D( [=]( ptr_t<node_t> self ){

        auto pos = self->get_attribute<transform_2D_t>( "transform" );
        auto mtx = self->get_root_viewport()->matrix.texture;
        auto tmp = shape_2D_t(); tmp.model = model;
        /*------*/ self->set_attribute( "shape", tmp );
        
        auto shp = self->get_attribute<shape_2D_t>( "shape" );

        self->onClose([=]( node_t* ){ model::unload( shp->model ); });
        self->on2D   ([=](){
    
            if  ( shp->mode & shape::MODE::SHAPE_MODE_FACES  ){ model::draw( shp->model, *pos, shp->color, shp->mode ); }
            elif( shp->mode & shape::MODE::SHAPE_MODE_EDGES  ){ model::draw( shp->model, *pos, shp->color, shp->mode ); }
            elif( shp->mode & shape::MODE::SHAPE_MODE_VERTEX ){ model::draw( shp->model, *pos, shp->color, shp->mode ); }
            
        });

    clb( self ); }); }

    /*─······································································─*/

    node_t node_polygon( int sides, function_t<void,ptr_t<node_t>> clb ) {

        ptr_t<vec3_t> point( sides );
        ptr_t<vec2_t> coord( sides );
        ptr_t<ushort> index( sides * 3 );
        
        for( int x=0; x<index.size()/3; x++ ){ 
             index[x*3+0] =   0;
             index[x*3+1] = x+1; 
             index[x*3+2] = x+2; 
        }

        for( int x=sides; x-->0; ){ 
             float y = ( 2 * PI * x/sides ) + PI / 4;
             point[x].x = sin( y );
             point[x].y = cos( y );
             point[x].z = 0;
             coord[x].x = sin( y ) * .5 + .5;
             coord[x].y = cos( y ) * .5 + .5;
        }

        return node_polygon( rl::LoadModelFromMesh( GenMeshRaw(
            sides, &point, &coord, &index
        )), clb ); 
    
    }

    /*─······································································─*/

    node_t node_line( function_t<void,ptr_t<node_t>> clb ) {
        ptr_t<vec3_t> point({ vec3_t({ 0, 0, 0 }), vec3_t({ 2, 0, 0 }) });
        return node_polygon( rl::LoadModelFromMesh( GenMeshRaw(
            1, &point, nullptr, nullptr
        )), clb ); 
    }

    /*─······································································─*/

    node_t node_rectangle( function_t<void,ptr_t<node_t>> clb ) {
    return node_polygon  ( 4, clb ); }

    node_t node_triangle( function_t<void,ptr_t<node_t>> clb ) { 
    return node_polygon ( 3, clb ); }

    node_t node_circle( function_t<void,ptr_t<node_t>> clb ) { 
    return node_polygon( 16, clb ); }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace node {

    node_t node_model( const model_t& model, function_t<void,ptr_t<node_t>> clb ) {
    return node_3D([=]( ptr_t<node_t> self ){

        auto pos = self->get_attribute<transform_3D_t>( "transform" );
        auto mtx = self->get_root_viewport()->matrix.texture;
        auto tmp = shape_3D_t(); tmp.model = model;
             self->set_attribute( "shape", tmp );

        auto shp = self->get_attribute<shape_3D_t>( "shape" );

        self->onClose([=]( node_t* ){ model::unload( shp->model ); });
        self->onU3D  ([=](){ 

            if  ( shp->mode & shape::MODE::SHAPE_MODE_FACES  ){ model::draw( shp->model, *pos, shp->color, shp->mode ); }
            elif( shp->mode & shape::MODE::SHAPE_MODE_EDGES  ){ model::draw( shp->model, *pos, shp->color, shp->mode ); }
            elif( shp->mode & shape::MODE::SHAPE_MODE_VERTEX ){ model::draw( shp->model, *pos, shp->color, shp->mode ); }
            
        });

    clb( self ); }); }

    /*─······································································─*/

    node_t node_model( string_t path, function_t<void,ptr_t<node_t>> clb ) {
    return node_model( model::load( path ), [=]( ptr_t<node_t> self ){

        auto pos = self->get_attribute<transform_3D_t>( "transform" );
        auto shp = self->get_attribute<shape_3D_t>( "shape" );
        shp->animation =  animation::load( path );

        self->onClose([=]( node_t* ){ animation::unload( shp->animation ); });

        self->onNext.add( coroutine::add( COROUTINE(){
            auto anm = &shp->animation;
            bool enb = ( anm->mode & animation::MODE::ANIMATION_MODE_PLAY    )==0;
            bool rvs = ( anm->mode & animation::MODE::ANIMATION_MODE_REVERSE )==0;
        coBegin

            if( anm->count == 0 ){ coEnd; } while( true ){
                animation::update( shp->model, *anm ); 
                anm->frame += rvs ? -anm->step : anm->step; 
                coWait( enb ); 
            coNext; }

        coFinish
        }));

    clb( self ); }); }

    /*─······································································─*/
    
    node_t node_ray( function_t<void,ptr_t<node_t>> clb ) {

        ptr_t<vec3_t> point({ vec3_t({ 0, 0, 0 }), vec3_t({ 0, 0, 2 }) });
        return node_polygon( model::load( GenMeshRaw(
            1, &point, nullptr, nullptr
        )), clb ); 

    }

    /*─······································································─*/
    
    node_t node_cone( function_t<void,ptr_t<node_t>> clb ) {
    return node_model( model::load( rl::GenMeshCone( 1, 1, 16 ) ), clb ); }
    
    node_t node_cube( function_t<void,ptr_t<node_t>> clb ) {
    return node_model( model::load( rl::GenMeshCube( 1, 1, 1 ) ), clb ); }

    node_t node_quad( function_t<void,ptr_t<node_t>> clb ) {
    return node_model( model::load( rl::GenMeshPlane( 1, 1, 1, 1 ) ), clb ); }

    node_t node_sphere( function_t<void,ptr_t<node_t>> clb ) {
    return node_model( model::load( rl::GenMeshSphere( 1, 16, 16 ) ), clb ); }
    
    node_t node_cylinder( function_t<void,ptr_t<node_t>> clb ) {
    return node_model( model::load( rl::GenMeshCylinder( 1, 1, 16 ) ), clb ); }
    
    node_t node_cubicmap( const image_t& img, function_t<void,ptr_t<node_t>> clb ) {
    return node_model( model::load( rl::GenMeshCubicmap( img, vec3_t({1,1,1}) ) ), clb ); }

    node_t node_heightmap( const image_t& img, function_t<void,ptr_t<node_t>> clb ) {
    return node_model( model::load( rl::GenMeshHeightmap( img, vec3_t({1,1,1}) ) ), clb ); }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/
