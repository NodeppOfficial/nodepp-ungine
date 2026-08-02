/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_GPU_KERNEL
#define UNGINE_GPU_KERNEL

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef GPU_KERNEL
#define GPU_KERNEL(...) #__VA_ARGS__
#endif

/*────────────────────────────────────────────────────────────────────────────*/

#if NODEPP_KERNEL == NODEPP_KERNEL_WASM
    #define GLSL_VERSION "#version 300 es\nprecision lowp float;\n"
#else 
#if defined(GRAPHICS_API_OPENGL_33)
    #define GLSL_VERSION "#version 330\n"
#elif defined(GRAPHICS_API_OPENGL_21)
    #define GLSL_VERSION "#version 120\n"
#else
    #define GLSL_VERSION "#version 300 es\nprecision lowp float;\n"
#endif
#endif

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace kernel {

    string_t& mk_default_kernel() { static string_t kernel = GPU_KERNEL(

        const ivec3 GSIZE   = ivec3( 32, 4, 4 );
        const float PI /**/ = 3.141592653589793;
        const float EPSILON = 0.01;

        uniform sampler2D roughness;
        uniform sampler2D metalness;
        uniform sampler2D oclussion;
        uniform sampler2D emission ;
        uniform sampler2D specular ;
        uniform sampler2D normal;
        uniform sampler2D albedo;
        uniform sampler2D global;

        uniform vec4 color;
        uniform mat4 ptx;
        uniform mat4 mtx;
        uniform mat4 vtx;
        uniform mat4 ttx;

        struct material_t {
            vec4  result, albedo  , canvas; float time;
            vec3  normal, position, size  ; 
            vec2  texel , uv, uv2 ;
        };

        struct vertex_t {
            vec4  result, color; float time;
            vec3  normal, size , position  ;
            vec2  texel , uv   , uv2;
        };

        vec4 pack( float value ) {
        uint b = floatBitsToUint(value); return vec4(
             float((b >> 24u) & 0xFFu) / 255.0,
             float((b >> 16u) & 0xFFu) / 255.0,
             float((b >> 8u ) & 0xFFu) / 255.0,
             float((b >> 0u ) & 0xFFu) / 255.0
        );}

        float unpack( vec4 color ) {
        uint  b = (uint(color.r * 255.0) << 24u) |
                  (uint(color.g * 255.0) << 16u) |
                  (uint(color.b * 255.0) << 8u ) |
                  (uint(color.a * 255.0) << 0u ) ;
        return uintBitsToFloat( b ); }

        vec4 mix( vec4 A, vec4 B ){ return A.a<EPSILON ? B : A ; }

        vec4 texture3D( sampler2D image, vec3 coord, int layers ){
            float  x_offset = coord.x ;
            float  y_offset = coord.y / float( layers );
            float  z_offset = coord.z / float( layers ); 
            return texture( image, vec2( x_offset, y_offset + z_offset) );
        }

        vec4 fetch3D( sampler2D image, ivec3 coord, ivec3 size ){ vec2 uv = vec2(
             float(coord.x) /*----------------------*/ / float(size.x),
            (float(coord.y) + float(coord.z * size.y)) / float(size.y * size.z)
        );  return texture( image, uv ); }

        vec3 render_size () { return vec3(
             unpack( fetch3D( global, ivec3( 0, 1, 3 ), GSIZE ) ),
             unpack( fetch3D( global, ivec3( 0, 1, 2 ), GSIZE ) ),
             unpack( fetch3D( global, ivec3( 0, 1, 1 ), GSIZE ) ) 
        ); }

    );  return kernel; }

    /*─······································································─*/

    string_t& cv_default_kernel() { static string_t kernel = GPU_KERNEL(
 
        in  vec2 vertexTexCoord2;
        in  vec2 vertexTexCoord ;
        in  vec3 vertexPosition ;
        in  vec3 vertexNormal   ;
        in  vec4 vertexColor    ;

        out vec2 fragTexCoord2;
        out vec2 fragTexCoord ;
        out vec3 fragPosition ;
        out vec3 fragNormal   ;
        out vec4 fragColor    ;

        void main(){
            gl_Position  = vec4( vertexPosition * 1.5, 1.0 );
            fragTexCoord = vec2((vertexTexCoord - 0.5) / 0.67 + 0.5 );
        }

    ); return kernel; }

} }

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace kernel {

    string_t& cf_default_kernel() { static string_t kernel = GPU_KERNEL(
        layout(location = 0) out vec4 gOutput; in vec2 fragTexCoord;

        vec4   albedo3D ( vec2 uv, int layer ) { 
        return texture3D( albedo , vec3( uv, float(layer) ), int( render_size().z ) ); }

        ${0} void main() { material_t mat;

            mat.time   = unpack( fetch3D( global, ivec3( 0, 1, 0 ), GSIZE ) );
            mat.texel  = 1. / render_size(   ).xy; mat.uv = fragTexCoord;
            mat.size   = /**/ render_size(   )   ;
            mat.albedo = albedo3D( mat.uv, 1 )   ;
            mat.canvas = albedo3D( mat.uv, 0 )   ;

            mat.result = mat.albedo.a<EPSILON ? mat.canvas : mat.albedo;
            gOutput    = _main_( mat ).result; 

        }
    
    ); return kernel; }

    /*─······································································─*/

    string_t& fs_default_kernel() { static string_t kernel = GPU_KERNEL(
        layout(location = 0) out vec4 gOutput;

        in vec2 fragTexCoord2;
        in vec2 fragTexCoord ;
        in vec3 fragPosition ;
        in vec3 fragNormal   ;
        in vec4 fragColor    ;

        ${0} void main() { material_t mat;

            int x = int( unpack( fetch3D( global, ivec3( 0, 0, 0 ), GSIZE ) ) ); 
        
            mat.time     = unpack ( fetch3D( global, ivec3( 0, 1, 0 ), GSIZE ) );
            mat.albedo   = texture( albedo, fract( fragTexCoord ) );
            mat.position = fragPosition ; mat.result = mat.albedo;
            mat.uv       = fragTexCoord ; mat.normal = fragNormal;
        //  mat.uv2      = fragTexCoord2;
            mat.texel    = 1. / render_size().xy;
            mat.size     = /**/ render_size()   ; 

            gOutput = _main_( mat, x ).result; 
        
        }

    ); return kernel; }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace kernel {
    
    string_t& vs_default_kernel() { static string_t kernel = GPU_KERNEL(
 
        in  vec2 vertexTexCoord2;
        in  vec2 vertexTexCoord ;
        in  vec3 vertexPosition ;
        in  vec3 vertexNormal   ;
        in  vec4 vertexColor    ;

        out vec2 fragTexCoord2;
        out vec2 fragTexCoord ;
        out vec3 fragPosition ;
        out vec3 fragNormal   ;
        out vec4 fragColor    ;

    ${0} void main() { vertex_t mat;

        mat.time      = unpack( fetch3D( global, ivec3( 0, 1, 0 ), GSIZE ) );
        mat.uv        = vertexTexCoord ; mat.color = vertexColor;
    //  mat.uv2       = vertexTexCoord2;
        mat.texel     = 1. / render_size().xy;
        mat.size      = /**/ render_size()   ; 
        mat.position  = (mtx * vec4(vertexPosition, 1.0)).xyz;
        mat.normal    = (mtx * vec4(vertexNormal  , 0.0)).xyz;
        mat.result    = (ttx * vec4(vertexPosition, 1.0));
        
        fragPosition  = mat.position;
        fragColor     = mat.color   ;
        fragTexCoord  = mat.uv      ;
    //  fragTexCoord2 = mat.uv2     ;
        fragNormal    = mat.normal  ;
        gl_Position   = _main_( mat ).result;

    }); return kernel; }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace kernel {

    string_t& fs_main_kernel() { static string_t kernel = NODEPP_STRINGIFY (
    material_t _main_( material_t mat, int layer ){ 
        if( layer!=1 ){ mat.result = vec4(0.); }
    return mat; }); return kernel; }

    /*─······································································─*/

    string_t& cf_main_kernel() { static string_t kernel = NODEPP_STRINGIFY (
    material_t _main_( material_t mat ){ return mat; }); return kernel; }

    /*─······································································─*/

    string_t& vs_main_kernel() { static string_t kernel = NODEPP_STRINGIFY (
    vertex_t  _main_( vertex_t mat ){ return mat; }); return kernel; }

} }

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/
