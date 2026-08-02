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

        shader_t shd = engine::get_default_model_shader();
        model_t  mdl = rl::LoadModelFromMesh( mesh );
        auto     vpt = engine::get_active_viewport();

        if( !shader::is_valid( shd ) ){ return mdl; }
        if( vpt.null() ) /*--------*/ { return mdl; }

        texture_t mtx = vpt->matrix.texture;

        for( int x = mdl.materialCount; x-->0; ){ 
             mdl.materials[x].maps[rl::MATERIAL_MAP_BRDF].texture = mtx;
             mdl.materials[x].shader = shd;
        }

    return mdl; }

    model_t load( string_t path ){ 

        shader_t shd = engine::get_default_model_shader();
        auto     vpt = engine::get_active_viewport();
        model_t  mdl = rl::LoadModel( path.get(  ) );

        if( !shader::is_valid( shd ) ){ return mdl; }
        if( vpt.null() ) /*--------*/ { return mdl; }

        texture_t mtx = vpt->matrix.texture;

        for( int x = mdl.materialCount; x-->0; ){ 
             mdl.materials[x].maps[rl::MATERIAL_MAP_BRDF].texture = mtx;
             mdl.materials[x].shader = shd;
        }

    return mdl; }

    /*─······································································─*/

    bool is_valid( const model_t& mdl ) { return rl::IsModelValid( mdl ); }

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

    void draw( const model_t& mdl, transform_3D_t trn, color_t tint, int mode ) {
        if( mode == ungine::shape::SHAPE_MODE_NONE ){ return; }
    rl::rlDisableBackfaceCulling(); do {

        vec3_t axs ({ 0.0f, 0.0f, 0.0f }); float ang = 0.0f;

        auto mtx = engine::get_active_viewport()->matrix;
        auto pos = trn.translate.position ;
        auto scl = trn.translate.scale    ;
        auto rot = rl::QuaternionFromEuler(
             trn.translate.rotation.x,
             trn.translate.rotation.y,
             trn.translate.rotation.z
        );

        if( mode & ungine::shape::SHAPE_MODE_WIRE ) 
          { rl::rlEnableWireMode(); }

        auto transform = math::matrix::from_transform_3D( trn );

        for( int x=0; x< mdl.meshCount; x++ ){
            
            color_t    col = { 0 };
            material_t mat = mdl.materials[mdl.meshMaterial[x]];
            color_t    cdl = mat.maps[rl::MATERIAL_MAP_DIFFUSE].color;

            col.r = (uchar)(((int)cdl.r*(int)tint.r)/255);
            col.g = (uchar)(((int)cdl.g*(int)tint.g)/255);
            col.b = (uchar)(((int)cdl.b*(int)tint.b)/255);
            col.a = (uchar)(((int)cdl.a*(int)tint.a)/255);
            mat.maps[rl::MATERIAL_MAP_DIFFUSE].color= col;

            if( matrix::get_pixel_float( mtx, 0, 0, 0 ) != 0.f 
            ) { rl::DrawMesh( mdl.meshes[x], mat, transform ); }

        }

        if( mode & ungine::shape::SHAPE_MODE_WIRE ) 
          { rl::rlDisableWireMode(); }

    } while(0); rl::rlEnableBackfaceCulling(); }

    /*─······································································─*/

    void draw( const model_t& mdl, transform_2D_t trn, color_t color, int mode ) {

        if( mode == ungine::shape::SHAPE_MODE_NONE ) { return; }
        vec3_t axs ({ 0.0f, 0.0f, 0.0f }); float ang = 0.0f;
        transform_3D_t tmp; mode |= 0x10000000;

        tmp.translate.position = vec3_t({
            trn.translate.position.x,
            trn.translate.position.y, 0
        });

        tmp.translate.scale    = vec3_t({
            trn.translate.scale.x,
            trn.translate.scale.y, 1
        });

        tmp.translate.rotation = vec3_t({
            0, 0, trn.translate.rotation
        });

        draw( mdl, tmp, color, mode );

    }

    /*─······································································─*/

    int unload( const model_t& mdl ){ 
        if ( !is_valid ( mdl ) ){ return -1; }
        rl::UnloadModel( mdl );   
    return 1; }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/
