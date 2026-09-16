/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_MATRIX
#define UNGINE_MATRIX

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace matrix {

    matrix_t load( int width, int height, int depth ) {
    matrix_t out = {};

        int format = image::FORMAT::FORMAT_UNCOMPRESSED_R8G8B8A8;

        out.texture = texture_t({
            rl::rlLoadTexture(NULL, width, height * depth, format, 1),
            width, height * depth, 1, format
        });

        ulong size = width * height * depth * 4;
        out.data   = (uchar*)RL_MALLOC( size );
        out.width  = width; out.height = height; out.depth = depth;

        memset( out.data, 0, size );
        rl::SetTextureFilter( out.texture, rl::TEXTURE_FILTER_POINT );

    return out; }

    void set_pixel_color( matrix_t matrix, ulong x, ulong y, ulong z, color_t data ) {
        ulong pos = ( x + ( matrix.width * y ) + ( matrix.width * matrix.height * z ) ) * 4;
        ulong mxx = matrix.width * matrix.height * matrix.depth * 4;
        
        if( pos + 4 > mxx ){ return; }
        uchar_32 *A= (uchar_32*)( matrix.data + pos ), 
                 *B= (uchar_32*)( &data ); *A = *B;

    }

    void set_pixel_float( matrix_t matrix, ulong x, ulong y, ulong z, float data ) {
        ulong pos = ( x + ( matrix.width * y ) + ( matrix.width * matrix.height * z ) ) * 4;
        ulong mxx = matrix.width * matrix.height * matrix.depth * 4;

        if( pos + 4 > mxx ) { return; }

        auto  pix = type::cast<uchar_32>( matrix.data + pos );
        auto  mem = (uchar_32*) &data;

        *pix = (uchar_32)(
            ( ( *mem >> 24 ) & 0xFF ) << 0  |
            ( ( *mem >> 16 ) & 0xFF ) << 8  |
            ( ( *mem >> 8  ) & 0xFF ) << 16 |
            ( ( *mem >> 0  ) & 0xFF ) << 24
        );

    }

    color_t get_pixel_color( matrix_t matrix, ulong x, ulong y, ulong z ) {
        ulong   pos = ( x + ( matrix.width * y ) + ( matrix.width * matrix.height * z ) ) * 4;
        ulong   mxx = matrix.width * matrix.height * matrix.depth * 4;
        color_t out; if( pos + 4 > mxx ){ return color_t({0}); }
        uchar_32 *B= (uchar_32*)( matrix.data + pos ),
                 *A= (uchar_32*)( &out ); *A = *B ; 
        return out;
    }

    float get_pixel_float( matrix_t matrix, ulong x, ulong y, ulong z ) {
        ulong pos= ( x + ( matrix.width * y ) + ( matrix.width * matrix.height * z ) ) * 4;
        ulong mxx= matrix.width * matrix.height * matrix.depth * 4;
        if( pos + 4 > mxx ){ return 0.0f; }

        auto  pix= type::cast<uchar_32>( matrix.data + pos ); float out;
        auto  mem= (uchar_32*) &out; 
        
        *mem = (uchar_32)(
            ( ( *pix >> 24 ) & 0xFF ) << 0  |
            ( ( *pix >> 16 ) & 0xFF ) << 8  |
            ( ( *pix >> 8  ) & 0xFF ) << 16 |
            ( ( *pix >> 0  ) & 0xFF ) << 24
        );

        return out;
        
    }

    void fill_pixel_float( matrix_t matrix, float data ) {
        ulong size = matrix.width * matrix.height * matrix.depth * 4;
        auto  mem  = (uchar_32*) &data;

        for( ulong x=0; x<size; x+=sizeof(uchar_32) ){
            auto pix = type::cast<uchar_32>( matrix.data + x ); 
            *pix = (uchar_32)(
                ( ( *mem >> 24 ) & 0xFF ) << 0  |
                ( ( *mem >> 16 ) & 0xFF ) << 8  |
                ( ( *mem >> 8  ) & 0xFF ) << 16 |
                ( ( *mem >> 0  ) & 0xFF ) << 24
            );
        } 

    }

    void fill_pixel_color( matrix_t matrix, color_t data ) {
        ulong size = matrix.width * matrix.height * matrix.depth * 4;
        for( ulong x=0; x<size; x+=sizeof(uchar_32) ){
            uchar_32 *A= (uchar_32*)( matrix.data + x );
            uchar_32 *B= (uchar_32*)( &data ); *A = *B ; 
        } 
    }

    void clear( matrix_t matrix ) {
        ulong size = matrix.width * matrix.height * matrix.depth * 4;
        memset( matrix.data, 0, size );
    }

    void update( matrix_t matrix ){
         rl::UpdateTexture( matrix.texture, matrix.data );
    }

    bool is_valid( matrix_t matrix ){
         return rl::IsTextureValid( matrix.texture );
    }

    void unload( matrix_t matrix ){
         if( !is_valid( matrix ) ){ return; }
         rl::UnloadTexture( matrix.texture );
         RL_FREE( matrix.data );
    }

} }

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/