/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_RENDER
#define UNGINE_RENDER

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace render {

    void begin    ( const render_t& render, int layer ){ rl::BeginTextureMode( render, layer ); }
    bool is_valid ( const render_t& render ){ return rl::IsGBufferValid( render ); }
    void end      () /*------------------*/ { rl::EndTextureMode  (); }
    uint get_layer() { return rl::GetRenderLayer(); }

    /*─······································································─*/

    render_t load( int width, int height, int depth ) {
        return rl::LoadGBuffer( width, height, depth );
    }

    render_t load( int depth ) {
        int width  = rl::GetRenderWidth ();
        int height = rl::GetRenderHeight();
        return rl::LoadGBuffer( width, height, depth );
    }

    render_t load() { return load( 1 ); }

    /*─······································································─*/

    int unload( const render_t& render ) {
         if( !is_valid( render ) )  { return -1; } 
         rl::UnloadGBuffer( render ); return  1;
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif
