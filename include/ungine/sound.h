/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_SOUND
#define UNGINE_SOUND

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace sound {

    sound_t load( string_t path ) { return rl::LoadSound( path.get() ); }

    /*─······································································─*/

    template< class T >
    sound_t load( const T& stream, string_t ext ) { do {

        auto data = stream::await(stream);
        if( !data.has_value() ){ break; }

        auto wave = rl::LoadWaveFromMemory( 
            ext.get(), 
            (uchar) data.value().get(), 
            /*---*/ data.value .size() 
        );

        auto snd = rl::LoadSoundFromWave( wave );
        rl::UnloadWave( wave ); return snd;

    } while(0); return {}; }

    /*─······································································─*/

    bool is_valid( sound_t& sound ) { 
         if( sound.frameCount==0 ){ return false; }
         return rl::IsSoundValid( sound ); 
    }

    bool is_playing( sound_t& sound ) {
         if( !is_valid( sound ) ){ return false; }
         return rl::IsSoundPlaying( sound ); 
    }

    /*─······································································─*/

    int set_volume( sound_t& sound, float val ) { 
        if( !is_valid( sound ) ) /*--*/ { return -1; }
        rl::SetSoundVolume( sound, val ); return  1; 
    }

    int set_pitch( sound_t& sound, float val ) { 
        if( !is_valid( sound ) ) /*-*/ { return -1; }
        rl::SetSoundPitch( sound, val ); return  1; 
    }

    int set_pan( sound_t& sound, float val ) { 
        if( !is_valid( sound ) )/*-*/{ return -1; }
        rl::SetSoundPan( sound, val ); return  1; 
    }

    /*─······································································─*/

    int play( sound_t& sound ) { 
        if( !is_valid( sound ) ){ return -1; }
        rl::PlaySound( sound );   return  1;
    }

    int pause( sound_t& sound ) { 
        if( !is_valid( sound ) ){ return -1; }
        rl::PauseSound( sound );  return  1;
    }

    int stop( sound_t& sound ) { 
        if( !is_valid( sound ) ){ return -1; }
        rl::StopSound( sound );   return  1;
    }

    int resume( sound_t& sound ) { 
        if( !is_valid( sound ) ){ return -1; }
        rl::ResumeSound( sound ); return  1;
    }

    /*─······································································─*/

    int unload( sound_t& sound ){ 
        if( !is_valid( sound ) ){ return -1; }
        rl::UnloadSound( sound ); return  1; 
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace music {

    music_t load( string_t path ) { return rl::LoadMusicStream( path.get() ); }

    /*─······································································─*/

    template< class T >
    music_t load( const T& stream, string_t ext ) { do {

        auto data = stream::await(stream);
        if( !data.has_value() ){ break; }

        return rl::LoadMusicStreamFromMemory( 
            ext.get(), 
            (uchar) data.value().get(), 
            /*---*/ data.value .size() 
        );

    } while(0); return {}; }

    /*─······································································─*/

    bool is_valid( const music_t& music ) { 
         if( music.frameCount==0 ){ return false; }
         return rl::IsMusicValid( music ); 
    }

    bool is_playing( const music_t& music ) {
         if( !is_valid( music ) ){ return false; }
         return rl::IsMusicStreamPlaying( music ); 
    }

    /*─······································································─*/

    int set_volume( const music_t& music, float val ) { 
        if( !is_valid( music ) ) /*--*/ { return -1; }
        rl::SetMusicVolume( music, val ); return  1; 
    }

    int set_pitch( const music_t& music, float val ) { 
        if( !is_valid( music ) ) /*-*/ { return -1; }
        rl::SetMusicPitch( music, val ); return  1; 
    }

    int set_pan( const music_t& music, float val ) { 
        if( !is_valid( music ) )/*-*/{ return -1; }
        rl::SetMusicPan( music, val ); return  1; 
    }

    /*─······································································─*/

    int play( const music_t& music ) { 
        if( !is_valid( music ) )/**/{ return -1; }
        rl::PlayMusicStream( music ); return  1;
    }

    int pause( const music_t& music ) { 
        if( !is_valid( music ) )/*-*/{ return -1; }
        rl::PauseMusicStream( music ); return  1;
    }

    int stop( const music_t& music ) { 
        if( !is_valid( music ) )/**/{ return -1; }
        rl::StopMusicStream( music ); return  1;
    }

    int resume( const music_t& music ) { 
        if( !is_valid( music ) )/*--*/{ return -1; }
        rl::ResumeMusicStream( music ); return  1;
    }

    int update( const music_t& music ) { 
        if( !is_valid( music ) )/*--*/{ return -1; }
        rl::UpdateMusicStream( music ); return  1;
    }

    /*─······································································─*/
    
    float seek( const music_t& music ) { 
        return rl::GetMusicTimePlayed( music ); 
    }

    float size( const music_t& music ) { 
        return rl::GetMusicTimeLength( music ); 
    }

    int tell( const music_t& music, float pos ) { 
        if( !is_valid( music ) )/*-----*/{ return -1; }
        rl::SeekMusicStream( music, pos ); return  1;
    }

    int next( const music_t& music ) { 
        if( !is_valid( music ) ) /*-------*/ { return -1; }
        float value = fabsf( seek(music) - size(music) );
        if( value<0.1f && is_playing(music) ){ return -2; } 
        update( music ); return 1;
    }

    /*─······································································─*/

    int unload( const music_t& music ){ 
        if( !is_valid( music ) )/*--*/{ return -1; }
        rl::UnloadMusicStream( music ); return  1; 
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace node { 

    node_t node_audio_3D( const music_t music, function_t<void,ptr_t<node_t>> clb ){
    return node_3D([=]( ptr_t<node_t> self ){

        self->set_attribute( "music", music );
        auto pos = self->get_attribute<transform_3D_t>("transform");
        auto snd = self->get_attribute<music_t>( "music" );
        auto vpt = self->get_viewport();

        self->onClose([=](){ music::unload( music ); });
        self->onNext.add( coroutine::add( COROUTINE(){
        coBegin

            if( music::is_valid( *snd ) ) { coEnd } while( true ){ 
                coWait( !music::is_playing( *snd ) ); 
            do {

                if( fabsf( music::seek( *snd ) - music::size( *snd ) ) < 0.1f &&
                    snd->looping && music::is_playing( *snd )
                ) { music::stop( *snd ); music::tell( *snd, 0.f ); break; }
                
                music::update( *snd );

            } while(0); coNext; }

        coFinish
        }));

        self->onLoop.add([=]( float delta ){
            if( vpt == nullptr ) /**/ { return -1; }
            if( vpt->camera3D.null() ){ return -1; }
            if( pos == nullptr ) /**/ { return -1; }

            auto cam_pos = vpt->camera3D->position;
            auto cam_tar = vpt->camera3D->target  ;

            float dist = math::distance( cam_pos, pos->position );
            float max_dist = 50.0f;
            float volume = 1.0f - ( dist / max_dist );
            if  ( volume < 0.0f ) { volume = 0.0f; }

            music::set_volume( *snd, volume * volume );

            vec3_t relative = Vector3Subtract ( pos->position, cam_pos );
            vec3_t forward  = Vector3Normalize( Vector3Subtract( cam_tar, cam_pos ) );
            vec3_t right    = Vector3Normalize( Vector3CrossProduct( forward, {0,1,0} ) );

            float pan = Vector3DotProduct( Vector3Normalize(relative), right );
            music::set_pan( *snd, ( pan + 1.0f ) / 2.0f );

        return 1; });

    clb( self ); }); } 

    /*─······································································─*/

    node_t node_audio( const music_t music, function_t<void,ptr_t<node_t>> clb ){
    return node_audio_3D( music, [=]( ptr_t<node_t> self ){
           self->onLoop.clear();
           self->remove_attribute( "transform" ); 
    clb( self ); }); } 

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/