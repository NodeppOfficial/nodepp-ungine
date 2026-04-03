/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_ANIMATION
#define UNGINE_ANIMATION

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace animation {

    animation_frame_t load( string_t path ){
        int /**/ count = 0; animation_frame_t out = {};
        out.animations = rl::LoadModelAnimations( path.get(), &count );
        out.count /**/ = count; return out;
    }

    /*─······································································─*/

    bool is_valid( const model_t& model, animation_frame_t anim ) {
         if( anim.animations==nullptr || anim.count==0 )
           { return false; } 
         uint index = anim.index % anim.count;
         animation_t addr = anim.animations[ index ]; 
         return rl::IsModelAnimationValid( model, addr ); 
    }

    void update( const model_t& model, animation_frame_t anim ) {
         if( !is_valid( model, anim ) ){ return; }
         uint index = anim.index % anim.count;
         animation_t addr = anim.animations[index];
         uint frame = anim.frame % addr.frameCount;
         rl::UpdateModelAnimation( model, addr, frame );
    }

    /*─······································································─*/

    int unload( animation_frame_t anim ){ 
        rl::UnloadModelAnimations( anim.animations, anim.count );
    return 1; }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/
