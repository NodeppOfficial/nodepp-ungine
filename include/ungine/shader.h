/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_SHADER
#define UNGINE_SHADER

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace gpu { 

template< class T > struct gpu_type_id  { static constexpr uchar value = 0xff; };

template<> struct gpu_type_id<int>      { static constexpr uchar value = 0x01; };
template<> struct gpu_type_id<bool>     { static constexpr uchar value = 0x02; };
template<> struct gpu_type_id<uint>     { static constexpr uchar value = 0x03; };
template<> struct gpu_type_id<float>    { static constexpr uchar value = 0x04; };

template<> struct gpu_type_id<bvec2_t>  { static constexpr uchar value = 0x11; };
template<> struct gpu_type_id<ivec2_t>  { static constexpr uchar value = 0x12; };
template<> struct gpu_type_id<uvec2_t>  { static constexpr uchar value = 0x13; };
template<> struct gpu_type_id<vec2_t>   { static constexpr uchar value = 0x14; };

template<> struct gpu_type_id<bvec3_t>  { static constexpr uchar value = 0x21; };
template<> struct gpu_type_id<ivec3_t>  { static constexpr uchar value = 0x22; };
template<> struct gpu_type_id<uvec3_t>  { static constexpr uchar value = 0x23; };
template<> struct gpu_type_id<vec3_t>   { static constexpr uchar value = 0x24; };

template<> struct gpu_type_id<bvec4_t>  { static constexpr uchar value = 0x31; };
template<> struct gpu_type_id<ivec4_t>  { static constexpr uchar value = 0x32; };
template<> struct gpu_type_id<uvec4_t>  { static constexpr uchar value = 0x33; };
template<> struct gpu_type_id<vec4_t >  { static constexpr uchar value = 0x34; };
template<> struct gpu_type_id<color_t>  { static constexpr uchar value = 0x35; };

template<> struct gpu_type_id<mat_t>    { static constexpr uchar value = 0x50; };
template<> struct gpu_type_id<texture_t>{ static constexpr uchar value = 0x51; };
template<> struct gpu_type_id<render_t >{ static constexpr uchar value = 0x52; };

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace shader {

    void set_variable( const shader_t& shader, string_t name, int /*unused*/, const texture_t* value, int /*unused*/ ) {
         int sid = rl::GetShaderLocation( shader, name.get() );
    if ( sid == -1 ){ return; }
         rl::SetShaderValueTexture( shader, sid, *value );
    }

    void set_variable( const shader_t& shader, string_t name, int /*unused*/, const mat_t* value, int count ) {
         int sid = rl::GetShaderLocation( shader, name.get() );
    if ( sid == -1 ){ return; }
         rl::rlEnableShader( shader.id );
         rl::rlSetUniformMatrices( sid, value, count );
    }

    template< class T >
    void set_variable( const shader_t& shader, string_t name, int flag, const T* value, int count ) {
         int sid = rl::GetShaderLocation( shader, name.get() );
    if ( sid == -1 ){ return; }
         rl::SetShaderValueV( shader, sid, value, flag, count );
    }

    /*─······································································─*/

    bool is_valid( const shader_t& shader ){ return rl::IsShaderValid( shader ); }
    void begin   ( const shader_t& shader ){ rl::BeginShaderMode( shader ); }
    void end     () /*------------------*/ { rl::EndShaderMode  (); }

    /*─······································································─*/

    template< class T >
    int set_attribute( const shader_t& shader, string_t name, const T* value, int count ){
    if( !is_valid( shader ) ){ return -1; } switch( gpu::gpu_type_id<T>::value ){
        case 0x04: set_variable( shader, name, gpu::ATTRIBUTE_FLOAT, value, count ); break;
        case 0x14: set_variable( shader, name, gpu::ATTRIBUTE_VEC2 , value, count ); break;
        case 0x24: set_variable( shader, name, gpu::ATTRIBUTE_VEC3 , value, count ); break;
        case 0x34: set_variable( shader, name, gpu::ATTRIBUTE_VEC4 , value, count ); break;
        case 0x35: set_variable( shader, name, gpu::ATTRIBUTE_VEC4 , value, count ); break;
        case 0x50: case 0x51:
        /*------*/ set_variable( shader, name, 0 /*-------------*/ , value, count ); break;
    }   return  1; }

    template< class T >
    int set_attribute( const shader_t& shader, string_t name, const T& value ){
        return set_attribute( shader, name, &value, 1 );
    }

    /*─······································································─*/

    template< class T >
    int set_uniform( const shader_t& shader, string_t name, const T* value, int count ){
    if( !is_valid( shader ) ){ return -1; } switch( (uchar) gpu::gpu_type_id<T>::value ){

        case 0x01: set_variable( shader, name, gpu::UNIFORM_BOOL , value, count ); break;
        case 0x02: set_variable( shader, name, gpu::UNIFORM_INT  , value, count ); break;
        case 0x03: set_variable( shader, name, gpu::UNIFORM_UINT , value, count ); break;
        case 0x04: set_variable( shader, name, gpu::UNIFORM_FLOAT, value, count ); break;

        case 0x11: set_variable( shader, name, gpu::UNIFORM_BVEC2, value, count ); break;
        case 0x12: set_variable( shader, name, gpu::UNIFORM_IVEC2, value, count ); break;
        case 0x13: set_variable( shader, name, gpu::UNIFORM_UVEC2, value, count ); break;
        case 0x14: set_variable( shader, name, gpu::UNIFORM_VEC2 , value, count ); break;

        case 0x21: set_variable( shader, name, gpu::UNIFORM_BVEC3, value, count ); break;
        case 0x22: set_variable( shader, name, gpu::UNIFORM_IVEC3, value, count ); break;
        case 0x23: set_variable( shader, name, gpu::UNIFORM_UVEC3, value, count ); break;
        case 0x24: set_variable( shader, name, gpu::UNIFORM_VEC3 , value, count ); break;

        case 0x31: set_variable( shader, name, gpu::UNIFORM_BVEC4, value, count ); break;
        case 0x32: set_variable( shader, name, gpu::UNIFORM_IVEC4, value, count ); break;
        case 0x33: set_variable( shader, name, gpu::UNIFORM_UVEC4, value, count ); break;
        case 0x34: set_variable( shader, name, gpu::UNIFORM_VEC4 , value, count ); break;
        
        case 0x35: set_variable( shader, name, gpu::UNIFORM_VEC4 , value, count ); break;
        case 0x50: case 0x51:
        /*------*/ set_variable( shader, name, 0 /*-----------*/ , value, count ); break;

    }   return  1; }

    template< class T >
    int set_uniform( const shader_t& shader, string_t name, const T& value ){
        return set_uniform( shader, name, &value, 1 );
    }

    /*─······································································─*/

    int set_location( const shader_t& shader, string_t name, uint location ) {
        if( name.empty() || location==-1 || !is_valid( shader ) ){ return -1; }
        int sid = rl::GetShaderLocation( shader, name.get() );
        if( sid==-1 ){ return -1; } shader.locs[location]=sid;
    return 1; }

    int get_location( const shader_t& shader, string_t name, uint location ) {
        if( name.empty() || location==-1 || !is_valid( shader ) ){ return -1; }
        return rl::GetShaderLocation( shader, name.get() );
    }

    /*─······································································─*/

    shader_t load( string_t vs_code, string_t fs_code ){

        auto kn = kernel::mk_default_kernel();
        auto vs = regex ::format( "${0}\n${2}\n${1}", GLSL_VERSION, vs_code, kn );
        auto fs = regex ::format( "${0}\n${2}\n${1}", GLSL_VERSION, fs_code, kn );

        auto sh = rl::LoadShaderFromMemory( 
               vs_code.empty() ? 0 : vs.get(),
               fs_code.empty() ? 0 : fs.get()
        ); 

        if( !shader::is_valid( sh ) ){ return sh; }

        shader::set_location( sh, "roughness", rl::SHADER_LOC_MAP_ROUGHNESS );
        shader::set_location( sh, "specular" , rl::SHADER_LOC_MAP_SPECULAR  );
        shader::set_location( sh, "emission" , rl::SHADER_LOC_MAP_EMISSION  );
        shader::set_location( sh, "albedo"   , rl::SHADER_LOC_MAP_DIFFUSE   );
        shader::set_location( sh, "game"     , rl::SHADER_LOC_MAP_BRDF      );
        shader::set_location( sh, "normal"   , rl::SHADER_LOC_MAP_NORMAL    );
        shader::set_location( sh, "mtx"      , rl::SHADER_LOC_MATRIX_MODEL  );
        shader::set_location( sh, "ttx"      , rl::SHADER_LOC_MATRIX_MVP    );
        shader::set_location( sh, "vtx"      , rl::SHADER_LOC_MATRIX_VIEW   );
        shader::set_location( sh, "ptx"      , rl::SHADER_LOC_MATRIX_PROJECTION );

    return sh; }

    /*─······································································─*/

    int unload( const shader_t& shader ){ 
        if( !is_valid( shader ) ) { return -1; }
        rl::UnloadShader( shader ); return  1; 
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/