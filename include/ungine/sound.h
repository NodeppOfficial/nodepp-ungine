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

#endif

/*────────────────────────────────────────────────────────────────────────────*/