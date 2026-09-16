/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_IMAGE
#define UNGINE_IMAGE

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace image {

    image_t load( const texture_t& texture ){ return rl::LoadImageFromTexture( texture ); }

    image_t load( string_t path ) { return rl::LoadImage( path.get() ); }

    /*─······································································─*/

    template< class T >
    image_t load( const T& stream, string_t ext ) { do {

        if( stream.is_close() ){ break; }

        auto data = stream::await(stream);
        if( !data.has_value() ){ break; }

        return rl::LoadImageFromMemory( 
            ext.get(), 
            (uchar) data.value().get(), 
            /*---*/ data.value .size() 
        );

    } while(0); return {}; }

    /*─······································································─*/

    bool is_valid( const image_t& image ) { return rl::IsImageValid( image ); }

    /*─······································································─*/

    ptr_t<uchar> raw_image( const image_t& image, string_t ext ) {
        if( !is_valid( image ) ){ return nullptr; } int size = 0;
        auto bff = rl::ExportImageToMemory( image, ".png", &size );
        return ptr_t<uchar>( bff, type::cast<ulong>( size ) );
    }

    int save_image( const image_t& image, string_t path ) {
        if( !is_valid( image ) ) { return false; }
        return rl::ExportImage( image, path.get() );
    }

    int set_format( const image_t& image, uint format ) { 
        if( !is_valid( image ) ){ return -1; }
        rl::ImageFormat( (image_t*) &image, format ); 
    return 1; }

    image_t copy( const image_t& image ) { 
        return image_t( rl::ImageCopy( image ) ); 
    }

    /*─······································································─*/

    int unload( const image_t& image ) {
        if( !is_valid( image ) ){ return -1; }
        rl::UnloadImage( image ); return  1;
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/
