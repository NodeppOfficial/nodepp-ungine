/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_MESH
#define UNGINE_MESH

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace mesh {

    model_t load( string_t path ) { return rl::LoadModel( path.get() ); }

    /*─······································································─*/

    bool is_valid( const model_t& model ) { return rl::IsModelValid( model ); }

    /*─······································································─*/

    int set_wrap_mode( const model_t& model, uint flag ) {
        if ( !is_valid( model ) ){ return -1; }
        for( auto x=model.materialCount; x--; ){
        for( auto y=12 ; y--; ){ // MAX_MATERIAL_MAPS = 12
             auto z=model.materials[x].maps[y].texture;
        if ( z.id > 0 ) { 
             rl::SetTextureWrap( z, flag );
        }}}
    return 1; }

    int set_texture_filter( const model_t& model, uint filter ) {
        if ( !is_valid( model ) ){ return -1; }
        for( auto x=model.materialCount; x--; ){
        for( auto y=12 ; y--; ){ // MAX_MATERIAL_MAPS = 12
             auto z=model.materials[x].maps[y].texture;
        if ( z.id > 0 ) { 
             rl:SetTextureFilter( z, filter );
        }}}
    return 1; }

    /*─······································································─*/

    void draw( const mesh_t& msh, transform_3D_t trn, material_t mat, color_t tint, int mode ) {
        if( mode == ungine::shape::SHAPE_MODE_NONE ){ return; }
    rl::rlDisableBackfaceCulling();

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

        color_t cdl = mat.maps[rl::MATERIAL_MAP_DIFFUSE].color;
        color_t col = { 0 };

        col.r = (uchar)(((int)cdl.r*(int)tint.r)/255);
        col.g = (uchar)(((int)cdl.g*(int)tint.g)/255);
        col.b = (uchar)(((int)cdl.b*(int)tint.b)/255);
        col.a = (uchar)(((int)cdl.a*(int)tint.a)/255);
        mat.maps[rl::MATERIAL_MAP_DIFFUSE].color= col;
        
        if( matrix::get_pixel_float( mtx, 0, 0, 0 ) != 0.f
        ) { rl::DrawMesh( msh, mat, transform ); }

        if( mode & ungine::shape::SHAPE_MODE_WIRE ) 
          { rl::rlDisableWireMode(); }

    rl::rlEnableBackfaceCulling(); }

    /*─······································································─*/

    void draw( const mesh_t& msh, transform_2D_t trn, material_t mat, color_t color, int mode ) {

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

        draw( msh, tmp, mat, color, mode );

    }

    /*─······································································─*/

    int unload( const model_t& model ){ 
        if( !is_valid( model ) ){ return -1; }
        rl::UnloadModel( model ); return  1; 
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/