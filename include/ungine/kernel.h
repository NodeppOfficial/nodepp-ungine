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

#if _KERNEL_ == NODEPP_KERNEL_WASM
    #define GLSL_VERSION "#version 300 es\nprecision mediump float;\n"
#else 
#if defined(GRAPHICS_API_OPENGL_33)
    #define GLSL_VERSION "#version 330\n"
#elif defined(GRAPHICS_API_OPENGL_21)
    #define GLSL_VERSION "#version 120\n"
#else
    #define GLSL_VERSION "#version 300 es\nprecision mediump float;\n"
#endif
#endif

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace kernel {

    string_t& mk_default_kernel() { static string_t kernel = GPU_KERNEL(

        const float PI      = 3.141592653589793;
        const float EPSILON = 0.01;

        float mean( vec4 data ){ return ( data.x + data.y + data.z + data.w ) / 4.; }
        float mean( vec3 data ){ return ( data.x + data.y + data.z ) / 3.; }
        float mean( vec2 data ){ return ( data.x + data.y ) / 2.; }

        ivec3 matrix_size() { return ivec3( 512, 4, 4 ); }

        vec4 pack( float value ) {
            uint b = floatBitsToUint(value); return vec4(
                float((b >> 24u) & 0xFFu) / 255.0,
                float((b >> 16u) & 0xFFu) / 255.0,
                float((b >> 8u ) & 0xFFu) / 255.0,
                float((b >> 0u ) & 0xFFu) / 255.0
            );
        }

        float unpack( vec4 color ) {
            uint b = (uint(color.r * 255.0) << 24u) |
                     (uint(color.g * 255.0) << 16u) |
                     (uint(color.b * 255.0) << 8u ) |
                     (uint(color.b * 255.0) << 0u ) ;
            return uintBitsToFloat( b );
        }

        vec4 texture3D( sampler2D image, vec3 coord, int layers ){
            float  x_offset = coord.x ;
            float  y_offset = coord.y / float( layers );
            float  z_offset = coord.z / float( layers ); 
            return texture( image, vec2( x_offset, y_offset + z_offset) );
        }

        vec4 fetch3D( sampler2D image, ivec3 coord, ivec3 size ){
             vec2 uv = vec2(
                float(coord.x) /*----------------------*/ / float(size.x),
               (float(coord.y) + float(coord.z * size.y)) / float(size.y * size.z)
             ); 
        return texture( image, uv ); }

    );  return kernel; }

} }

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace kernel {

    string_t& cf_default_kernel() { static string_t kernel = GPU_KERNEL(
    
    layout(location = 0) out vec4 gOutput;

    uniform sampler2D albedo;
    uniform sampler2D normal;
    uniform sampler2D game  ;
    in   vec2  fragTexCoord ;

    struct scene_t {
        vec3  albedo  , normal   , position, stencil;
        float specular, roughness, emission;
        float model   , depth    , ldepth  ;
    };

    struct camera_t {
        vec3  position , direction;
        float fog, near, far, fovy;
    };

    struct light_t {
        vec3  color, position, direction, scale;
        float wrap , energy  ;
        int   mode ;
    };

    vec3 ambient_light( light_t light, scene_t scene, camera_t cam ){
         return light.color * light.energy / 100.;
    }

    vec3 sun_light( light_t light, scene_t scene, camera_t cam ){
        
        vec3 sdir  = normalize(-light.direction );
        vec3 vdir  = normalize( cam.position );
        vec3 rdir  = reflect  (-sdir, scene.normal );

        float diff = max(dot( scene.normal, sdir ), 0.0 );
        float dist = length ( light.position - scene.position );
        float spec = pow(max(dot( vdir,rdir ), 0.0), scene.roughness );

        float result = ( diff + ( spec * scene.specular ) );
        float energy = light.energy / 100.;

        return light.color * result * energy;

    }

    vec3 point_light( light_t light, scene_t scene, camera_t cam ){

        vec3 ldir  = normalize( light.position - scene.position );
        vec3 vdir  = normalize( cam.position );
        vec3 rdir  = reflect  (-ldir, scene.normal );

        float dist = length ( light.position - scene.position );
        float diff = max(dot( scene.normal, ldir ), 0.0 );
        float attn = light.energy / ( dist * dist + 1.0 );
        float spec = pow(max(dot( vdir, rdir ), 0.0), scene.roughness );
        float result = ( diff + ( spec * scene.specular ) ) * attn;

        return light.color * result;
        
    }

    vec3 spot_light( light_t light, scene_t scene, camera_t cam ){

        vec3 lpos    = normalize( scene.position - light.position );
        vec3 sdir    = normalize(-light.direction );
        float theta  = dot( lpos, sdir );
        float cutoff = cos( light.wrap );

        if( theta <= cutoff ){ return vec3( 0. ); }
        float wrap = clamp((theta-cutoff)/EPSILON, 0.0, 1.0);

        return point_light( light, scene, cam ) * wrap;

    }

    vec3 area_light( light_t light, scene_t scene, camera_t cam ) {

        vec3 forward = normalize(light.direction);
        vec3 right   = normalize(cross(vec3(0.0, 1.0, 0.0), forward));
        if( length(right) <= 0. ){ right = vec3( 1.0, 0.0, 0.0 ); }
        vec3 up      = cross( forward, right );

        vec3 rel_pos = scene.position - light.position;
        vec3 scale   = light.scale * 0.5;
        
        vec3 local_pos = vec3(
            dot(rel_pos, right  ),
            dot(rel_pos, up     ),
            dot(rel_pos, forward)
        );

        vec3 closest_local     = clamp( local_pos, -scale, scale );
        vec3 light_surface_pos = /*-----*/ light.position  + 
                                 right   * closest_local.x + 
                                 up      * closest_local.y + 
                                 forward * closest_local.z;

        if( abs(local_pos.x) > scale.x + EPSILON || 
            abs(local_pos.y) > scale.y + EPSILON || 
            abs(local_pos.z) > scale.z + EPSILON ){
            return vec3(0.0);
        }

        return light.color;
    }

    void main() {

        ivec3   size  = matrix_size();
        vec3    light = vec3( 0. ); scene_t scn; camera_t cam; light_t lgh;
        int MAX_LIGHT = int( unpack( fetch3D( game, ivec3( 5, 0, 3 ), size ) ) );

        do {

            scn.normal     = texture3D( normal, vec3( fragTexCoord, 1 ), 7 ).rgb * 2.0 - 1.0;
            scn.roughness  = .1; //texture3D( normal, vec3( fragTexCoord, 0 ), 7 ).r;
            scn.specular   = texture3D( normal, vec3( fragTexCoord, 0 ), 7 ).g;
            scn.emission   = texture3D( normal, vec3( fragTexCoord, 0 ), 7 ).b;
            scn.model      = texture3D( normal, vec3( fragTexCoord, 1 ), 7 ).a;
            scn.albedo     = texture3D( normal, vec3( fragTexCoord, 5 ), 7 ).rgb;
            scn.stencil    = texture3D( normal, vec3( fragTexCoord, 6 ), 7 ).rgb;

            scn.position.x = unpack( texture3D( normal, vec3( fragTexCoord, 2 ), 7 ) );
            scn.position.y = unpack( texture3D( normal, vec3( fragTexCoord, 3 ), 7 ) );
            scn.position.z = unpack( texture3D( normal, vec3( fragTexCoord, 4 ), 7 ) );

        } while( false );

        do{

            cam.position.x = unpack( fetch3D( game, ivec3( 3, 0, 0 ), size ) );
            cam.position.y = unpack( fetch3D( game, ivec3( 3, 0, 1 ), size ) );
            cam.position.z = unpack( fetch3D( game, ivec3( 3, 0, 2 ), size ) );

            cam.direction.x= unpack( fetch3D( game, ivec3( 4, 0, 0 ), size ) );
            cam.direction.y= unpack( fetch3D( game, ivec3( 4, 0, 1 ), size ) ); 
            cam.direction.z= unpack( fetch3D( game, ivec3( 4, 0, 2 ), size ) );

            cam.fog        = unpack( fetch3D( game, ivec3( 3, 0, 3 ), size ) );

            cam.near       = unpack( fetch3D( game, ivec3( 5, 0, 0 ), size ) ); 
            cam.far        = unpack( fetch3D( game, ivec3( 5, 0, 1 ), size ) );
            cam.fovy       = unpack( fetch3D( game, ivec3( 5, 0, 2 ), size ) );

        } while( false );

        do{ if( scn.model==0. ){ scn.ldepth = 1.; scn.depth = 1.; break; }

            float dist = length ( cam.position - scn.position );
            float linearDepth = (dist-cam.near)/(cam.far-cam.near);
            scn.ldepth = clamp  ( linearDepth, 0.0, 1.0 );
        
            vec3 viewDir = normalize( cam.direction ); 
            scn.depth    = dot( cam.position - scn.position, viewDir ) / cam.far;

        } while( false );
        
        int x=0; do{

            lgh.color       = fetch3D( game, ivec3( x, 1, 3 ), size ).rgb * scn.model;
            lgh.mode        = int( fetch3D( game, ivec3( x, 1, 3 ), size ).a * 255. );
            lgh.wrap        = unpack( fetch3D( game, ivec3( x, 2, 3 ), size ) );
            lgh.energy      = unpack( fetch3D( game, ivec3( x, 3, 3 ), size ) );

            lgh.position .x = unpack( fetch3D( game, ivec3( x, 1, 0 ), size ) );
            lgh.position .y = unpack( fetch3D( game, ivec3( x, 1, 1 ), size ) );
            lgh.position .z = unpack( fetch3D( game, ivec3( x, 1, 2 ), size ) );

            lgh.scale    .x = unpack( fetch3D( game, ivec3( x, 2, 0 ), size ) );
            lgh.scale    .y = unpack( fetch3D( game, ivec3( x, 2, 1 ), size ) );
            lgh.scale    .z = unpack( fetch3D( game, ivec3( x, 2, 2 ), size ) );

            lgh.direction.x = unpack( fetch3D( game, ivec3( x, 3, 0 ), size ) );
            lgh.direction.y = unpack( fetch3D( game, ivec3( x, 3, 1 ), size ) );
            lgh.direction.z = unpack( fetch3D( game, ivec3( x, 3, 2 ), size ) );

            switch( lgh.mode ){
                case 1: light += point_light  ( lgh, scn, cam ); break;
                case 2: light += area_light   ( lgh, scn, cam ); break;
                case 3: light += ambient_light( lgh, scn, cam ); break;
                case 4: light += spot_light   ( lgh, scn, cam ); break;
                case 5: light += sun_light    ( lgh, scn, cam ); break;
            }

        x++; } while( x < MAX_LIGHT ); // light = light / float( MAX_LIGHT );

        if( scn.emission > 0. ){ 
            gOutput = vec4( scn.albedo, scn.model ); 
        } else {
            gOutput = vec4( scn.albedo* light, scn.model );
        }

    //  gOutput = vec4( scn.albedo, 1. ); // << FIXME

    }

    ); return kernel; }

    string_t& cv_default_kernel() { static string_t kernel = GPU_KERNEL(
 
        in      vec3 vertexPosition;
        in      vec2 vertexTexCoord;
        in      vec3 vertexNormal  ;
        in      vec4 vertexColor   ;

        out     vec2 fragTexCoord;
        out     vec3 fragPosition;
        out     vec3 fragNormal  ;
        out     vec4 fragColor   ;

        uniform sampler2D game   ;
        uniform mat4 ptx;
        uniform mat4 mtx;
        uniform mat4 vtx;
        uniform mat4 ttx;

        void main(){

            gl_Position  = vec4( vertexPosition * 1.5, 1.0 );
            fragTexCoord = vec2((vertexTexCoord - 0.5) / 0.67 + 0.5 );

        }

    ); return kernel; }

    string_t& fs_default_kernel() { static string_t kernel = GPU_KERNEL(

        layout(location = 0) out vec4 gOutput;

        uniform sampler2D roughness;
        uniform sampler2D specular ;
        uniform sampler2D emission ;
        uniform sampler2D albedo   ;
        uniform sampler2D normal   ;
        uniform sampler2D game     ;

        in      vec2 fragTexCoord;
        in      vec3 fragPosition;
        in      vec3 fragNormal  ;
        in      vec4 fragColor   ;
        
        uniform float mde;
        uniform mat4  ptx;
        uniform mat4  mtx;
        uniform mat4  vtx;
        uniform mat4  ttx;

        void main(){

            switch( int( unpack( fetch3D( game, ivec3( 0, 0, 0 ), matrix_size() ) ) ) ){

                case 0: do { gOutput = vec4( 0. ); } while( false ); break;

                case 1: do {

                    gOutput = vec4( 
                        texture( roughness, fragTexCoord ).r,
                        texture( specular , fragTexCoord ).r,
                        texture( emission , fragTexCoord ).r, 1.
                    );

                } while( false ); break;

                case 2: do {
                    gOutput = vec4( ( normalize( fragNormal ) ) * 0.5 + 0.5, 1. );
                } while( false ); break;

                case 3: do {
                    gOutput   = pack( vec4( fragPosition, 1. ).x );
                    gOutput.a = 1.; 
                } while( false ); break;

                case 4: do {
                    gOutput   = pack( vec4( fragPosition, 1. ).y );
                    gOutput.a = 1.;
                } while( false ); break;

                case 5: do {
                    gOutput   = pack( vec4( fragPosition, 1. ).z );
                    gOutput.a = 1.;
                } while( false ); break;

                case 6: do {
                    gOutput = vec4( texture( albedo, fract( fragTexCoord ) ).rgb, 1. );
                } while( false ); break;

                case 7: do {
                    
                    vec4  raw = fetch3D( game, ivec3( 6, 0, 0 ), matrix_size() );
                    float  ID = raw.r * 255.;
                    float MASK= raw.g * 255.;
                    
                    if( ID == 0. ){ 
                        gOutput = vec4( 0. );
                    } else {
                        gOutput = vec4( 1. );
                    }

                } while( false ); break;

            }

        //  gOutput = vec4( 1. ); // << FIXME
        
        }

    ); return kernel; }

    string_t& vs_default_kernel() { static string_t kernel = GPU_KERNEL(
 
        in      vec3 vertexPosition;
        in      vec2 vertexTexCoord;
        in      vec3 vertexNormal  ;
        in      vec4 vertexColor   ;

        out     vec2 fragTexCoord;
        out     vec3 fragPosition;
        out     vec3 fragNormal  ;
        out     vec4 fragColor   ;

        uniform sampler2D game   ;
        uniform mat4 ptx;
        uniform mat4 mtx;
        uniform mat4 vtx;
        uniform mat4 ttx;

        void main(){

            fragTexCoord = vertexTexCoord;
            fragColor    = vertexColor   ;
        //  fragPosition = vertexPosition;
        
            gl_Position  = (ttx * vec4(vertexPosition, 1.0));
            fragPosition = (mtx * vec4(vertexPosition, 1.0)).xyz;
            fragNormal   = (mtx * vec4(vertexNormal  , 0.0)).xyz;

        }

    ); return kernel; }

} }

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/
