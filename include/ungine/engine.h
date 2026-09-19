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

    event_t<>      onConstructor;
    event_t<>      onDestructor ;

    event_t<>      onExit ;
    event_t<>      onOpen ;
    event_t<>      onNext ;
    event_t<float> onLoop ;
    event_t<>      onClose;
    event_t<>      onDraw ;

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace engine {

    ptr_t<viewport_t>& get_active_viewport() {
    static ptr_t<viewport_t> out ( 0UL );
    return out; }

    shader_t& get_default_model_shader() {
    static shader_t out = shader::load( 
        kernel::vs_default_kernel(), 
        kernel::fs_default_kernel(),
        kernel::vs_main_kernel   (),
        kernel::fs_main_kernel   ()
    );  return out; }

    shader_t& get_default_canva_shader() {
    static shader_t out = shader::load( 
        kernel::cv_default_kernel(),
        kernel::cf_default_kernel(),
        nullptr /*---------------*/,
        kernel::cf_main_kernel   ()
    );  return out; }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace engine {

    bool   is_ready() /*--*/ { return onConstructor.empty() && onDestructor.empty(); }
    bool   should_close()    { return rl::WindowShouldClose(); }
    float  get_delta() /*-*/ { return rl::GetFrameTime(); }
    
    float& get_time()        { static float out=0.f; return out; }
    void   set_fps( int fps ){ rl::SetTargetFPS( fps ); }
    int    get_fps() /*---*/ { return rl::GetFPS(); }

    void close() { 
        static bool b=0; if( b ){ return; } b=1;
        /*-----------*/ rl::CloseAudioDevice();
        shader ::unload( get_default_canva_shader() );
        shader ::unload( get_default_model_shader() );
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

        process::add( coroutine::add( COROUTINE(){
        coBegin; coWait( !rl::IsWindowReady() ); onOpen.emit(); 
            
            while( !should_close() ){ 
            do{if( !is_ready    () ){ break; }

                onLoop.emit( get_delta() );
                onNext.emit( /*-------*/ );
                onDraw.emit( /*-------*/ );
                get_time()+= get_delta()  ;

            } while(0); 
            
                if  ( !onDestructor .empty() ){ onDestructor .emit(); } 
                elif( !onConstructor.empty() ){ onConstructor.emit(); }
            
            coNext; } close();

        coFinish }));

    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/