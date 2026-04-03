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

    int set_texture_filter( const model_t& model, uint filter ) {
        if ( !is_valid( model ) ){ return -1; }
        for( auto x=model.materialCount; x--; ){
        for( auto y=12 ; y--; ){ // MAX_MATERIAL_MAPS = 12
             auto z=model.materials[x].maps[y].texture;
        if ( z.id > 0 ) { 
             rl:SetTextureFilter( z, filter );
        }}}
    return 1; }

    int set_wrap_mode( const model_t& model, uint flag ) {
        if ( !is_valid( model ) ){ return -1; }
        for( auto x=model.materialCount; x--; ){
        for( auto y=12 ; y--; ){ // MAX_MATERIAL_MAPS = 12
             auto z=model.materials[x].maps[y].texture;
        if ( z.id > 0 ) { 
             rl::SetTextureWrap( z, flag );
        }}}
    return 1; }

    /*─······································································─*/

    void draw( const model_t& model, transform_3D_t trn, color_t color, int mode ) {
    rl::rlDisableBackfaceCulling();

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
        rl::DrawModelEx( model, pos, axs, ang*RAD2DEG, scl, color );
    }

    elif( mode & ungine::shape::SHAPE_MODE_EDGES ){

        rl::QuaternionToAxisAngle( rot, &axs, &ang );
        rl::DrawModelWiresEx( model, pos, axs, ang*RAD2DEG, scl, color );

    }

    elif( mode & ungine::shape::SHAPE_MODE_VERTEX ){

        rl::QuaternionToAxisAngle( rot, &axs, &ang );
        rl::DrawModelPointsEx( model, pos, axs, ang*RAD2DEG, scl, color );

    }

    rl::rlEnableBackfaceCulling(); }

    /*─······································································─*/

    int unload( const model_t& model ){ 
        if( !is_valid( model ) ){ return -1; }
        rl::UnloadModel( model ); return  1; 
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/