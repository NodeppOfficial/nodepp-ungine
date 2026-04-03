/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_ENGINE
#define UNGINE_ENGINE

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace engine {

    event_t<>      onExit ;
    event_t<>      onOpen ;
    event_t<>      onNext ;
    event_t<float> onLoop ;
    event_t<>      onClose;
    event_t<>      onDraw ;
    global_t       global ;
    lock_t         locker ;

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace engine {

    texture_t& get_default_texture() {
        static texture_t out; if( texture::is_valid( out ) ){ return out; }
        image_t img = rl::GenImageColor( 10, 10, rl::BLANK );
        out = texture::load( img ); image::unload( img ); 
        texture::set_filter( out, texture::FILTER::TEXTURE_FILTER_POINT );
        return out;
    }

    shader_t& get_default_model_shader() {
        static shader_t out = shader::load( 
            kernel::vs_default_kernel(), 
            kernel::fs_default_kernel() 
        );  return out;
    }

    shader_t& get_default_canva_shader() {
        static shader_t out = shader::load( 
            kernel::cv_default_kernel(),
            kernel::cf_default_kernel()
        );  return out;
    }

    ptr_t<viewport_t>& get_active_viewport() {
        static ptr_t<viewport_t> out ( 0UL );
        return out; 
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace engine {

    bool is_ready() /*----*/ { return rl::IsWindowReady() || !locker.is_locked(); }

    bool  should_close()     { return rl::WindowShouldClose(); }

    float get_delta() /*--*/ { return rl::GetFrameTime(); }

    void  set_fps( int fps ) { rl::SetTargetFPS( fps ); }

    int   get_fps() /*----*/ { return rl::GetFPS(); }

    void close() { 
        static bool b=0; if( b ){ return; } b=1;
        /*-----------*/ rl::CloseAudioDevice();
        shader ::unload( get_default_canva_shader() );
        shader ::unload( get_default_model_shader() );
        texture::unload( get_default_texture     () );
        onClose.emit(); rl::CloseWindow(); 
        onExit .emit(); process::exit(1);
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace engine {

    void start( int width, int height, string_t title ) {

        rl::InitWindow( width, height, title.get() );
        rl::rlSetClipPlanes( 0.1, 500 );
        rl::InitAudioDevice();
    //  rl::SetExitKey(0);

        process::onSIGEXIT([](){ close(); });

        process::add( coroutine::add( COROUTINE(){
        coBegin ; coWait( !is_ready() );

            onOpen.emit(); 
            
            while( !should_close() ){

                coWait/*-*/( !is_ready() );
                onNext.emit( /*-------*/ );
                onLoop.emit( get_delta() );
                onDraw.emit( /*-------*/ );

            coNext; } close();

        coFinish
        }));

    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif
