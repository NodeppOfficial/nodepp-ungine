/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_CAMERA
#define UNGINE_CAMERA

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace cam3D {

    void begin( camera_3D_t render ){ rl::BeginMode3D( render ); }
    void end  () /*--------------*/ { rl::EndMode3D  (); }

    ray_t vec_2_world( camera_3D_t cam, vec2_t pos ) {
        auto tmp = rl::Camera3D();
             tmp.position   = cam.position;
             tmp.target     = cam.target;
             tmp.up         = cam.up;
             tmp.fovy       = cam.fovy;
             tmp.projection = cam.projection;
        return rl::GetScreenToWorldRay( pos, tmp );
    }

    vec2_t world_2_vec( camera_3D_t cam, vec3_t pos ) {
        auto tmp = rl::Camera3D();
             tmp.position   = cam.position;
             tmp.target     = cam.target;
             tmp.up         = cam.up;
             tmp.fovy       = cam.fovy;
             tmp.projection = cam.projection;
        return rl::GetWorldToScreen( pos, tmp );
    }

}}

namespace ungine { namespace cam2D {

    void begin( camera_2D_t render ){ rl::BeginMode2D( render ); }
    void end  () /*--------------*/ { rl::EndMode2D  (); }

    vec2_t vec_2_world( camera_2D_t cam, vec2_t pos ) {
        return rl::GetScreenToWorld2D( pos, cam );
    }

    vec2_t world_2_vec( camera_2D_t cam, vec2_t pos ) {
        return rl::GetWorldToScreen2D( pos, cam );
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace node { node_t node_camera_3D( function_t<void,ptr_t<node_t>> clb ){
return node_3D([=]( ptr_t<node_t> self ){

    camera_3D_t camera;
    /*-------*/ camera.fovy       = 90.0f;
    /*-------*/ camera.projection = camera::VIEW::PROJECTION_VIEW_PERSPECTIVE;

    self->set_attribute( "camera", camera );

    auto view = self->get_viewport(); if( view==nullptr ) { return; }

    if( view->camera3D.null() ){
        view->camera3D = self->get_attribute<camera_3D_t>( "camera" );
    }

    auto pos = self->get_attribute<transform_3D_t>( "transform" );
    auto cam = self->get_attribute<camera_3D_t>   ( "camera" );

    self->onLoop([=]( float /*unused*/ ){

        auto rot = math::matrix::rotation( math::negate( pos->translate.rotation ) );
        auto fr  = vec3_t({ rot.m2, rot.m6, rot.m10 });
        auto up  = vec3_t({ rot.m1, rot.m5, rot.m9  });

        cam->target   = pos->translate.position + rl::Vector3Normalize(fr);
        cam->position = pos->translate.position ;
        cam->up       = rl::Vector3Normalize(up);

    });

clb( self ); }); }}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace node { node_t node_fly_camera_3D( function_t<void,ptr_t<node_t>> clb ){
return node_camera_3D([=]( ptr_t<node_t> self ){

    auto pos = self->get_attribute<transform_3D_t>( "transform" );
    auto cam = self->get_attribute<camera_3D_t>   ( "camera" );
    auto vec = type::bind( vec3_t({ 0, 0, 0 }) );

    self->onLoop([=]( float delta ){

        if  ( key::is_down( 'W' ) )
            { vec->z = 300 * delta * 1; }
        elif( key::is_down( 'S' ) )
            { vec->z = 300 * delta *-1; }
        else{ vec->z = 0; }

        if  ( key::is_down( 'A' ) )
            { vec->x = 300 * delta * 1; }
        elif( key::is_down( 'D' ) )
            { vec->x = 300 * delta *-1; }
        else{ vec->x = 0; }

    });

    self->onLoop([=]( float delta ){
    if( !window::is_focused() ){ return; }

        auto data /*-*/ = mouse::get_delta();
        pos->rotation  += vec3_t({data.y,-data.x,0.f}) * delta * .5f;
        pos->rotation.x = clamp( pos->rotation.x, -PI/2, PI/2 );

        pos->position += math::vec3::rotation( *vec, 
        /*------------*/ math::quaternion::from_euler( pos->rotation ) 
        ) * delta * 10.f;

    });

clb( self ); }); }}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace node { node_t node_camera_2D( function_t<void,ptr_t<node_t>> clb ){
return node_2D([=]( ptr_t<node_t> self ){

    camera_2D_t camera; camera.zoom = 1.0f;

    self->set_attribute( "camera", camera );
    auto view = self->get_viewport(); if( view==nullptr ) { return; }

    if( view->camera2D.null() ){
        view->camera2D = self->get_attribute<camera_2D_t>( "camera" );
    }

    auto pos = self->get_attribute<transform_2D_t>( "transform" );
    auto cam = self->get_attribute<camera_2D_t>   ( "camera" );

    self->onLoop([=]( float /*unused*/ ){

        cam->offset   = window::normalized({.5f,.5f});
        cam->target   = pos->translate.position; 
        cam->rotation = pos->translate.rotation;

    });

clb( self ); }); }}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace node { node_t node_fly_camera_2D( function_t<void,ptr_t<node_t>> clb ){
return node_camera_2D([=]( ptr_t<node_t> self ){

    auto cam = self->get_attribute<camera_2D_t>   ( "camera" );
    auto pos = self->get_attribute<transform_2D_t>( "transform" );

    self->onLoop([=]( float delta ){
    if( !cursor::is_hidden() ){ return; }

        if  ( key::is_down( 'W' ) )
            { pos->position.y += 100 * delta * 1; }
        elif( key::is_down( 'S' ) )
            { pos->position.y += 100 * delta *-1; }

        if  ( key::is_down( 'A' ) )
            { pos->position.x += 100 * delta * 1; }
        elif( key::is_down( 'D' ) )
            { pos->position.x += 100 * delta *-1; }

    });

clb( self ); }); }}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/
