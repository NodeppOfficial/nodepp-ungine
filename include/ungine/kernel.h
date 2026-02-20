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

/* -- CANVAS_SHADERS -- */
namespace ungine { namespace kernel {

    string_t& cv_default_kernel() { static string_t kernel =  GPU_KERNEL(

        varying vec2 fragTexCoord;
        uniform sampler2D screen ;
        uniform sampler2D depth  ;
        uniform int vrMode       ;

        void main() {

            float distortion    = 0.5 ; // -1. ~ 1.
            float aberration    = 0.05; //  0. ~ 1.
            float edge_softness = 0.01; //  0. ~ 1.
            
            if( vrMode==0 ){
                gl_FragColor = texture2D(screen, fragTexCoord);
            return; }

            float isRight = step(0.5, fragTexCoord.x);
            vec2       uv = vec2(fract(fragTexCoord.x * 2.0), fragTexCoord.y);
            vec2    delta = uv - 0.5; /*------*/ float r2 = dot(delta, delta);

            vec2 distorted_uv   = uv + delta * (distortion * r2);
            vec2 final_uv_red   = vec2((uv + delta * (distortion * r2 + aberration * r2)).x * 0.5 + (isRight * 0.5), distorted_uv.y);
            vec2 final_uv_green = vec2( distorted_uv.x * 0.5 + (isRight * 0.5) /*-------------------------------*/ , distorted_uv.y);
            vec2 final_uv_blue  = vec2((uv + delta * (distortion * r2 - aberration * r2)).x * 0.5 + (isRight * 0.5), distorted_uv.y);

            vec3 pix = vec3(
                 texture2D( screen, final_uv_red   ).r,
                 texture2D( screen, final_uv_green ).g,
                 texture2D( screen, final_uv_blue  ).b
            );

            vec2 border = smoothstep(1.0, 1.0 - edge_softness, distorted_uv)
                        * smoothstep(0.0, /*-*/ edge_softness, distorted_uv);

            float square_mask = border.x * border.y;
            gl_FragColor = vec4(pix,1.0) * square_mask;

        }

    ); return kernel; }

} }

/*────────────────────────────────────────────────────────────────────────────*/

/* -- FRAGMENT_SHADERS -- */
namespace ungine { namespace kernel {

    string_t& fs_default_kernel() { static string_t kernel =  GPU_KERNEL(

        varying vec2 fragTexCoord;
        varying vec3 fragPosition;
        varying vec3 fragNormal  ;
        varying vec4 fragColor   ;

        uniform sampler2D img;

        uniform vec3 cam;
        uniform vec3 slf;
        uniform vec3 lgh;

        void main(){
            gl_FragColor = texture2D( img, fragTexCoord );
        //  gl_FragColor = abs( vec4( fragPosition, 1. ) );
        }

    ); return kernel; }

} }

/*────────────────────────────────────────────────────────────────────────────*/

/* -- VERTEX_SHADERS -- */
namespace ungine { namespace kernel {

    string_t& vs_default_kernel() { static string_t kernel = GPU_KERNEL(

        attribute vec3 vertexPosition;
        attribute vec2 vertexTexCoord;
        attribute vec3 vertexNormal  ;
        attribute vec4 vertexColor   ;

        uniform mat4 mvp;

        varying vec2 fragTexCoord;
        varying vec3 fragPosition;
        varying vec3 fragNormal  ;
        varying vec4 fragColor   ;

        void main(){

            fragTexCoord = vertexTexCoord;
            fragPosition = vertexPosition;
            fragNormal   = vertexNormal  ;
            fragColor    = vertexColor   ;
            gl_Position  = mvp*vec4(vertexPosition, 1.0);

        }

    ); return kernel; }

} }

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/
