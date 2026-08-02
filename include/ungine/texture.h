/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_TEXTURE
#define UNGINE_TEXTURE

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace texture {

    texture_t load( const image_t& image ){ return rl::LoadTextureFromImage( image ); }

    bool is_valid( const texture_t& texture ) { return rl::IsTextureValid( texture ); }

    texture_t load( string_t path ) { return rl::LoadTexture( path.get() ); }

    /*─······································································─*/

    template< class T >
    texture_t load( const T& stream, string_t ext ) { do {

        if( stream.is_closed() ){ break; }

        auto data = stream::await(stream);
        if( !data.has_value() ){ break; }

        auto img = rl::LoadImageFromMemory( 
            ext.get(), 
            ( uchar ) data.value().get(), 
            /*-----*/ data.value .size() 
        );

        auto txt = rl::LoadTextureFromImage( img );
        rl::UnloadImage( img ); return txt;

    } while(0); return texture_t(); }

    /*─······································································─*/

    void begin( const texture_t& texture ){ 
        rl::SetShapesTexture( texture, rect_t({
            0.f, 0.f, (float)texture.width, 
            /*-----*/ (float)texture.height
        }) ); 
    }

    void end(){ 
        rl::SetShapesTexture( texture_t(), rect_t({
            0.f, 0.f, 0.f, 0.f
        }) ); 
    }

    /*─······································································─*/

    int set_wrap_mode( const texture_t& texture, uint filter ) {
         if( !is_valid( texture ) ){ return -1; }
         rl::SetTextureWrap( texture, filter );
    return 1; }

    int set_filter( const texture_t& texture, uint filter ) {
         if( !is_valid( texture ) ){ return -1; }
         rl::SetTextureFilter( texture, filter );
    return 1; }

    /*─······································································─*/

    void draw( const texture_t& texture, rect_t pos, rect_t src, float angle=0 ){
        auto origin = vec2_t({ pos.width, pos.height }) / 2; 
        rl::DrawTexturePro( texture, src, pos, origin, angle*RAD2DEG, rl::WHITE );
    }

    /*─······································································─*/

    int unload( const texture_t& texture ){ 
        if( !is_valid( texture ) )  { return -1; }
        rl::UnloadTexture( texture ); return  1; 
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/
