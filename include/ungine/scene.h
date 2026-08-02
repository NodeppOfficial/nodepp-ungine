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
    void begin   ( rect_t rec ){ rl::BeginScissorMode( rec.x, rec.y, rec.width, rec.height ); }
    void end     () /*------*/ { rl::EndScissorMode  (); }
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
node_t node_render( function_t<void,ptr_t<node_t>> clb, int layers ) {
return node::node_rectangle( [=]( ptr_t<node_t> self ){

    auto rnd = ptr_t<render_t>( 0UL, render::load(layers) );

    auto shp = self->get_attribute<shape_2D_t>( "shape" );
    auto mtx = matrix::load( 32 , 4, 4 );
    auto tmp = viewport_t(); 
         tmp.render = rnd; 
         tmp.matrix = mtx;
    
    shp->model.materials[0].shader /*-----*/ = engine::get_default_canva_shader();
    shp->model.materials[0].maps[rl::MATERIAL_MAP_ALBEDO].texture = rnd[0].albedo;
    shp->model.materials[0].maps[rl::MATERIAL_MAP_BRDF  ].texture = mtx.texture  ;

    self->set_attribute( "viewport", tmp ); self->on2D.clear();
    auto  vpt = self->get_attribute<viewport_t>( "viewport" );
    
    self->onClose([=](){ render::unload(rnd[0]); matrix::unload( mtx ); }); 
    self->onNext ([=](){
        auto &tmp = engine::get_active_viewport(); tmp = vpt;
    }); self->onNext.emit();

    if( &vpt == self->get_root_viewport() ){
    self->onNext.add( coroutine::add( COROUTINE(){
    coBegin

        do {

            auto  mtx = vpt->matrix; 
            float stm = engine::get_delta()
                      + matrix::get_pixel_float( mtx, 0, 0, 1 ); 

            matrix::set_pixel_float( mtx, 0, 1, 3, window::get_size().x );
            matrix::set_pixel_float( mtx, 0, 1, 2, window::get_size().y );
            matrix::set_pixel_float( mtx, 0, 1, 1, tmp.render[0].depth  );
            matrix::set_pixel_float( mtx, 0, 1, 0, stm );

        } while(0); coGoto(0);
        
    coFinish })); }

    self->onDraw ([=](){

        auto vpt = self->get_attribute<viewport_t>( "viewport" );
        auto que = self->get_render_queue();
        auto mtx = vpt->matrix;

    for( int x=0; x<=vpt->render[0].depth; x++ ){

        matrix::set_pixel_float( mtx, 0, 0, 0, (float) x );
        matrix::update( mtx );
        render::begin ( vpt->render[0], x );

        if( x==0 ){ rl::ClearBackground( rl::BLANK ); }
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
node_t node_scene( function_t<void,ptr_t<node_t>> clb, int layers=2 ) {
return node_render([=]( ptr_t<node_t> self ){

    auto pos = self->get_attribute<transform_2D_t>( "transform" );
    auto vpt = self->get_attribute<viewport_t>    ( "viewport" );
    auto shp = self->get_attribute<shape_2D_t>    ( "shape" );
    auto mtx = vpt ->matrix;

    self->onDraw([=](){
      
        draw::begin(); rl::ClearBackground ( vpt->background );
        model::draw( shp->model, *pos, shp->color, shp->mode );

        /*
        blend::begin( blend::MODE::BLEND_MODE_ALPHA );
        rl::DrawTexturePro( vpt->render[0].albedo,
            rect_t({ 0, 0, 
                (float) vpt->render[0].albedo.width ,
                (float)-vpt->render[0].albedo.height,
            }), 
            rect_t({ 0, 0, 
                (float) window::get_size().x,
                (float) window::get_size().y,
            }), 
        vec2_t({ 0, 0 }), .0f, rl::WHITE );
        blend::end();
        */
        
    draw::end(); }); clb( self );
 
}, layers ); }}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/
