/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_MODEL
#define UNGINE_MODEL

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace model {

    model_t load( mesh_t mesh ){ 

        shader_t  shd = engine::get_default_model_shader();
        texture_t txt = engine::get_default_texture();
        model_t   mdl = rl::LoadModelFromMesh( mesh );
        auto      vpt = engine::get_active_viewport();

        if( !shader::is_valid( shd ) ){ return mdl; }
        if( vpt.null() ) /*--------*/ { return mdl; }

        texture_t mtx = vpt->matrix.texture;

        for( int x = mdl.materialCount; x-->0; ){ 
             mdl.materials[x].maps[rl::MATERIAL_MAP_BRDF].texture = mtx;
        for( int y = 12; /*----------*/ y-->0; ){
        if ( !texture::is_valid( mdl.materials[x].maps[y].texture ) ){
             mdl.materials[x].maps[y].texture = txt;
        }  } mdl.materials[x].shader /*----*/ = shd; }

    return mdl; }

    model_t load( string_t path ){ 

        shader_t  shd = engine::get_default_model_shader();
        texture_t txt = engine::get_default_texture();
        model_t   mdl = rl::LoadModel( path.get() );
        auto      vpt = engine::get_active_viewport();

        if( !shader::is_valid( shd ) ){ return mdl; }
        if( vpt.null() ) /*--------*/ { return mdl; }

        texture_t mtx = vpt->matrix.texture;

        for( int x = mdl.materialCount; x-->0; ){ 
             mdl.materials[x].maps[rl::MATERIAL_MAP_BRDF].texture = mtx;
        for( int y = 12; /*----------*/ y-->0; ){
        if ( !texture::is_valid( mdl.materials[x].maps[y].texture ) ){
             mdl.materials[x].maps[y].texture = txt;
        }  } mdl.materials[x].shader /*----*/ = shd; }

    return mdl; }

    /*─······································································─*/

    bool is_valid( const model_t& mdl ) { return rl::IsModelValid( mdl ); }

    /*─······································································─*/

    void set_stencil_id( const model_t& mdl, uchar ID, uchar MASK ) { 

        float value; color_t color ({ ID, MASK, 255, 255 });
        memcpy( &value, &color, sizeof( color_t ) );

        for( int x =  mdl.materialCount; x-->0; ){ 
            auto z = &mdl.materials[x].maps[rl::MATERIAL_MAP_BRDF];
            z->value = value;
        }

    }

    void update( const model_t& mdl ) {
         auto ID  = mdl.materials[0].maps[rl::MATERIAL_MAP_BRDF];
         auto mtx = engine::get_active_viewport()->matrix ;
         matrix::set_pixel_float( mtx, 6, 0, 0, ID.value );
         matrix::set_pixel_color( mtx, 6, 0, 1, ID.color );
         matrix::update( mtx );
    }

    /*─······································································─*/

    int set_wrap_mode( const model_t& mdl, uint flag ) {
        if ( !is_valid( mdl ) ){ return -1; }
        for( int  x=mdl.materialCount; x--; ){
        for( int  y=12 ; y-->0; ){ // MAX_MATERIAL_MAPS = 12
             auto z=mdl.materials[x].maps[y].texture;
        if ( z.id > 0 ) { 
             rl::SetTextureWrap( z, flag );
        }}}
    return 1; }

    int set_texture_filter( const model_t& mdl, uint filter ) {
        if ( !is_valid( mdl ) ){ return -1; }
        for( int  x=mdl.materialCount; x--; ){
        for( int  y=12 ; y-->0; ){ // MAX_MATERIAL_MAPS = 12
             auto z=mdl.materials[x].maps[y].texture;
        if ( z.id > 0 ) { 
             rl::SetTextureFilter( z, filter );
        }}}
    return 1; }

    /*─······································································─*/

    void draw( const model_t& mdl, transform_2D_t trn, color_t color, int mode ) {
    rl::rlDisableBackfaceCulling(); update( mdl );
    rl::BeginBlendMode( 0 ); do {

        auto rot = rl::QuaternionFromEuler(
             0, 0, trn.translate.rotation
        );
        
        auto scl = vec3_t({
             trn.translate.scale.x,
             trn.translate.scale.y, 1
        });

        auto pos = vec3_t({
             trn.translate.position.x,
             trn.translate.position.y, 0
        });

        vec3_t axs ({ 0.0f, 0.0f, 0.0f }); float ang = 0.0f;

    if( mode & ungine::shape::SHAPE_MODE_FACES ) { 
        rl::QuaternionToAxisAngle( rot, &axs, &ang );
        rl::DrawModelEx( mdl, pos, axs, ang*RAD2DEG, scl, color );
    }

    elif( mode & ungine::shape::SHAPE_MODE_EDGES ){

        rl::QuaternionToAxisAngle( rot, &axs, &ang );
        rl::DrawModelWiresEx( mdl, pos, axs, ang*RAD2DEG, scl, color );

    }

    elif( mode & ungine::shape::SHAPE_MODE_VERTEX ){

        rl::QuaternionToAxisAngle( rot, &axs, &ang );
        rl::DrawModelPointsEx( mdl, pos, axs, ang*RAD2DEG, scl, color );

    }

    } while(0); rl::EndBlendMode();
    rl::rlEnableBackfaceCulling(); }

    /*─······································································─*/

    void draw( const model_t& mdl, transform_3D_t trn, color_t color, int mode ) {
    rl::rlDisableBackfaceCulling(); update( mdl );
    rl::BeginBlendMode( 0 ); do {

        auto mtx = engine::get_active_viewport()->matrix;

        if( int( matrix::get_pixel_float( mtx, 0, 0, 0 ) ) == 7 &&
                 matrix::get_pixel_float( mtx, 6, 0, 0 )   == 0.
        ) { break; }

        auto rot = rl::QuaternionFromEuler(
             trn.translate.rotation.x,
             trn.translate.rotation.y,
             trn.translate.rotation.z
        );
        
        auto scl = trn.translate.scale;
        auto pos = trn.translate.position;

        vec3_t axs ({ 0.0f, 0.0f, 0.0f }); float ang = 0.0f;

    if( mode & ungine::shape::SHAPE_MODE_FACES ) { 
        rl::QuaternionToAxisAngle( rot, &axs, &ang );
        rl::DrawModelEx( mdl, pos, axs, ang*RAD2DEG, scl, color );
    }

    elif( mode & ungine::shape::SHAPE_MODE_EDGES ){

        rl::QuaternionToAxisAngle( rot, &axs, &ang );
        rl::DrawModelWiresEx( mdl, pos, axs, ang*RAD2DEG, scl, color );

    }

    elif( mode & ungine::shape::SHAPE_MODE_VERTEX ){

        rl::QuaternionToAxisAngle( rot, &axs, &ang );
        rl::DrawModelPointsEx( mdl, pos, axs, ang*RAD2DEG, scl, color );

    }

    } while(0); rl::EndBlendMode();
    rl::rlEnableBackfaceCulling(); }

    /*─······································································─*/

    int unload( const model_t& mdl ){ 
        if ( !is_valid ( mdl ) ){ return -1; }
        rl::UnloadModel( mdl );   
    return 1; }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/
