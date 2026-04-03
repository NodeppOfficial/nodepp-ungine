/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_SCENE
#define UNGINE_SCENE

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace scissor {
    void begin( rect_t rec ){ rl::BeginScissorMode( rec.x, rec.y, rec.width, rec.height ); }
    void end  () /*------*/ { rl::EndScissorMode  (); }
}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace blend {
    void begin   ( uint blend_mode ){ rl::BeginBlendMode( blend_mode ); }
    void end     () /*-----------*/ { rl::EndBlendMode  (); }
}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace draw {
    void begin() { rl::BeginDrawing(); }
    void end  () { rl::EndDrawing  (); }
}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace node { 
node_t node_render( function_t<void,ptr_t<node_t>> clb ) {
return node::node_rectangle( [=]( ptr_t<node_t> self ){

    auto rnd = ptr_t<render_t>({ render::load(1), render::load(7) });
    auto shp = self->get_attribute<shape_2D_t>( "shape" );
    auto mtx = matrix::load( 512, 4, 4 );
    auto stm = ptr_t<float>( 0UL, 0.0f );
    auto tmp = viewport_t(); 
         tmp.render = rnd; 
         tmp.matrix = mtx;
    
    shp->model.materials[0].shader /*-----*/ = engine::get_default_canva_shader();
    shp->model.materials[0].maps[rl::MATERIAL_MAP_ALBEDO].texture = rnd[0].albedo;
    shp->model.materials[0].maps[rl::MATERIAL_MAP_NORMAL].texture = rnd[1].albedo;
    shp->model.materials[0].maps[rl::MATERIAL_MAP_BRDF  ].texture = mtx.texture  ;

    self->set_attribute( "viewport", tmp ); self->on2D.clear();
    auto  vpt = self->get_attribute<viewport_t>( "viewport" );

    self->onNext([=](){
        auto &tmp = engine::get_active_viewport(); tmp = vpt;
    }); self->onNext.emit();

    self->onClose([=]( node_t* ){ for( auto x: rnd ){ 
        render::unload(x); 
    }   matrix::unload( mtx ); }); 

    if( &vpt == self->get_root_viewport() ){
    self->onNext.add( coroutine::add( COROUTINE(){
    coBegin ; coWait( vpt->camera3D.null() );

        do{ vec3_t frw = vpt->camera3D->position - vpt->camera3D->target;
            float d = engine::get_delta(); *stm += d;
            int   x = 0, y = 0; auto mtx = vpt->matrix;

            matrix::set_pixel_float( mtx, 3, 0, 0, vpt->camera3D->position.x );
            matrix::set_pixel_float( mtx, 3, 0, 1, vpt->camera3D->position.y );
            matrix::set_pixel_float( mtx, 3, 0, 2, vpt->camera3D->position.z );
            matrix::set_pixel_float( mtx, 3, 0, 3, vpt->camera3D->fog  );

            matrix::set_pixel_float( mtx, 2, 0, 0,*stm );
            matrix::set_pixel_float( mtx, 1, 0, 0, d   );

            matrix::set_pixel_float( mtx, 4, 0, 0, frw.x );
            matrix::set_pixel_float( mtx, 4, 0, 1, frw.y );
            matrix::set_pixel_float( mtx, 4, 0, 2, frw.z );

            matrix::set_pixel_float( mtx, 5, 0, 0, vpt->camera3D->near );
            matrix::set_pixel_float( mtx, 5, 0, 1, vpt->camera3D->far  );
            matrix::set_pixel_float( mtx, 5, 0, 2, vpt->camera3D->fovy );
            
        } while(0); coGoto(0);
        
    coFinish })); }

    self->onDraw ([=](){

        auto vpt = self->get_attribute<viewport_t>( "viewport" );
        auto que = self->get_render_queue();
        auto mtx = vpt->matrix;

    for( int x=0; x<=vpt->render[1].depth; x++ ){

        do{ matrix::set_pixel_float( mtx, 0, 0, 0, (float) x );
            matrix::update( mtx );
        } while(0);

        if ( x==0 ){ 
            render::begin( vpt->render[0], x );
            rl::ClearBackground( rl::BLANK );
        } else { int y = x-1;
            render::begin( vpt->render[1], y );
        if ( y==0 ){ 
            rl::ClearBackground( rl::BLANK ); 
        }}

        if( x==7 ){ 
            rl::rlEnableDepthTest(); 
            rl::rlEnableDepthMask();
        }

        if( !vpt->camera3D.null() ){ do {
        cam3D::begin( *vpt->camera3D );

            auto w=que->eventU3D.first(); while( w!=nullptr ){
            auto y=w->next; w->data.emit(); w=y; }

        if( x!=0 ){ break; }

            auto z=que->event3D.first(); while( z!=nullptr ){
            auto y=z->next; z->data.emit(); z=y; }

        } while(0); cam3D::end(); }

        if( !vpt->camera2D.null() ){ do {
        cam2D::begin( *vpt->camera2D );

            auto w=que->eventU2D.first(); while( w!=nullptr ){
            auto y=w->next; w->data.emit(); w=y; }

        if( x!=0 ){ break; }

            auto z=que->event2D.first(); while( z!=nullptr ){
            auto y=z->next; z->data.emit(); z=y; }

        } while(0); cam2D::end(); } else { do {

            auto w=que->eventU2D.first(); while( w!=nullptr ){
            auto y=w->next; w->data.emit(); w=y; }

        if( x!=0 ){ break; }

            auto z=que->event2D.first(); while( z!=nullptr ){
            auto y=z->next; z->data.emit(); z=y; }

        } while(0); } do {

            auto w=que->eventUUI.first(); while( w!=nullptr ){
            auto y=w->next; w->data.emit(); w=y; }

        if( x!=0 ){ break; }

            auto z=que->eventUI.first(); while( z!=nullptr ){
            auto y=z->next; z->data.emit(); z=y; }

        } while(0);

    render::end(); }}); clb( self ); 

}); }}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace node {
node_t node_scene( function_t<void,ptr_t<node_t>> clb ) {
return node_render([=]( ptr_t<node_t> self ){

    self->onDraw([=](){

        auto pos = self->get_attribute<transform_2D_t>( "transform" );
        auto vpt = self->get_attribute<viewport_t>( "viewport" );
        auto shp = self->get_attribute<shape_2D_t>( "shape" );
        auto mtx = vpt ->matrix;
      
        draw::begin(); rl::ClearBackground( vpt->background );

        blend::begin( blend::MODE::BLEND_MODE_ALPHA );
        model::draw( shp->model, *pos, shp->color, shp->mode );
        matrix::clear( mtx );
        blend::end();

        blend::begin( blend::MODE::BLEND_MODE_ALPHA );
        rl::DrawTexturePro( 
            vpt->render[0].albedo,
            rect_t({ 
                0, 0, (float) vpt->render[0].albedo.width ,
                      (float)-vpt->render[0].albedo.height,
            }), 
            rect_t({ 
                0, 0, (float) window::get_size().x,
                      (float) window::get_size().y,
            }), 
        vec2_t({ 0, 0 }), .0f, rl::WHITE );
        blend::end();
        
    draw::end(); }); clb( self );
 
}); }}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif
