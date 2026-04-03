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

        int size  = width * height * depth * 4;
        out.data  = (uchar*)RL_MALLOC( size * sizeof(uchar));
        out.width = width; out.height = height; out.depth = depth;

        memset( out.data, 0, size * sizeof(uchar) );
        rl::SetTextureFilter( out.texture, rl::TEXTURE_FILTER_POINT );

    return out; }

    void set_pixel_color( matrix_t matrix, int x, int y, int z, color_t data ) {
        int pos = ( x + ( matrix.width * y ) + ( matrix.width * matrix.height * z ) ) * 4;
        int mxx = matrix.width * matrix.height * matrix.depth * 4;
        if( pos < 0 || pos > mxx - 4 ) { return; }
        matrix.data[ pos + 0 ] = data.r;
        matrix.data[ pos + 1 ] = data.g;
        matrix.data[ pos + 2 ] = data.b;
        matrix.data[ pos + 3 ] = data.a;
    }

    void set_pixel_float( matrix_t matrix, int x, int y, int z, float data ) {
        int pos = ( x + ( matrix.width * y ) + ( matrix.width * matrix.height * z ) ) * 4;
        int mxx = matrix.width * matrix.height * matrix.depth * 4;
        if( pos < 0 || pos > mxx - 4 ) { return; }

        uint res = 0; memcpy( &res, &data, sizeof(uint) );

        matrix.data[ pos + 0 ] = (uchar)((res >> 24) & 0xFF);
        matrix.data[ pos + 1 ] = (uchar)((res >> 16) & 0xFF);
        matrix.data[ pos + 2 ] = (uchar)((res >> 8 ) & 0xFF);
        matrix.data[ pos + 3 ] = (uchar)((res >> 0 ) & 0xFF);
    }

    color_t get_pixel_color( matrix_t matrix, int x, int y, int z ) {
        int pos = ( x + ( matrix.width * y ) + ( matrix.width * matrix.height * z ) ) * 4;
        return color_t({ 
            matrix.data[ pos + 0 ], matrix.data[ pos + 1 ],
            matrix.data[ pos + 2 ], matrix.data[ pos + 3 ]
        });
    }

    float get_pixel_float( matrix_t matrix, int x, int y, int z ) {
        int pos = ( x + ( matrix.width * y ) + ( matrix.width * matrix.height * z ) ) * 4;
        
        uint res = 0;
             res |= (uint)matrix.data[ pos + 0 ] << 24;
             res |= (uint)matrix.data[ pos + 1 ] << 16;
             res |= (uint)matrix.data[ pos + 2 ] << 8 ;
             res |= (uint)matrix.data[ pos + 3 ] << 0 ;

        float  out = 0; memcpy( &out, &res, sizeof(float) );
        return out;
    }

    void fill_pixel_float( matrix_t matrix, float data ) {
        int  size = matrix.width * matrix.height * matrix.depth;
        uint res  = 0; memcpy( &res, &data, sizeof(uint) );

        for( uint x=0; x<size; x++ ){
             matrix.data[ x * 4 + 0 ] = (uchar)((res >> 24) & 0xFF);
             matrix.data[ x * 4 + 1 ] = (uchar)((res >> 16) & 0xFF);
             matrix.data[ x * 4 + 2 ] = (uchar)((res >> 8 ) & 0xFF);
             matrix.data[ x * 4 + 3 ] = (uchar)((res >> 0 ) & 0xFF);
        } 

    }

    void fill_pixel_color( matrix_t matrix, color_t data ) {
        int size = matrix.width * matrix.height * matrix.depth;
        for( uint x=0; x<size; x++ ){
             matrix.data[ x * 4 + 0 ] = data.r;
             matrix.data[ x * 4 + 1 ] = data.g;
             matrix.data[ x * 4 + 2 ] = data.b;
             matrix.data[ x * 4 + 3 ] = data.a;
        } 
    }

    void clear( matrix_t matrix ) {
        int size = matrix.width * matrix.height * matrix.depth;
        memset( matrix.data, 0, size * 4 * sizeof(uchar) );
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