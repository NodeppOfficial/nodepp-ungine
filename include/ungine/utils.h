
/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_UTILS
#define UNGINE_UTILS

/*────────────────────────────────────────────────────────────────────────────*/

/*
namespace ungine { namespace rl {
rl::RayCollision GetRayCollisionMesh( Ray ray, Mesh mesh, Matrix transform ){
rl::RayCollision collision = { 0 };

    // Check if mesh vertex data on CPU for testing
    if( mesh.vertices != NULL ) {
        int triangleCount = mesh.triangleCount;

        // Test against all triangles in mesh
    for( int i = 0; i < triangleCount; i++ ){
        Vector3 a, b, c;
        Vector3 *vertdata = (Vector3 *)mesh.vertices;

        if( mesh.indices){
            a = vertdata[mesh.indices[i*3 + 0]];
            b = vertdata[mesh.indices[i*3 + 1]];
            c = vertdata[mesh.indices[i*3 + 2]];
        } else {
            a = vertdata[i*3 + 0];
            b = vertdata[i*3 + 1];
            c = vertdata[i*3 + 2];
        }

        a = Vector3Transform(a, transform);
        b = Vector3Transform(b, transform);
        c = Vector3Transform(c, transform);

        RayCollision triHitInfo = GetRayCollisionTriangle(ray, a, b, c);

        if( triHitInfo.hit ){
            // Save the closest hit triangle
            if ((!collision.hit) || (collision.distance > triHitInfo.distance)) collision = triHitInfo;
        }
    
    }}

    return collision;
}}}
*/

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace rl {

    uint& GetRenderLayer() {
        static uint layer; return layer; 
    }

    void SetRenderLayer( uint layer ){
        uint* raw = &GetRenderLayer();
        *raw = layer;
    }

    void ClearRenderLayer() { SetRenderLayer(0); }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace rl { 
void BeginTextureMode( GBufferRenderTexture target, int layer ) {

    rlDrawRenderBatchActive(/*-*/);      
    rlEnableFramebuffer(target.id); 
    rlViewport( 
        0 /*------*/, target.height * layer,
        target.width, target.height
    );
    
    SetRenderLayer( layer );

}}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace rl {     
bool IsGBufferValid( GBufferRenderTexture target ) {

    if( target.id == 0 ) /*--------------*/ { return false; }
    if( !IsTextureValid( target.albedo   ) ){ return false; }
    if( !IsTextureValid( target.depthness) ){ return false; }

return true; }
}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace rl {     
void UnloadGBuffer( const GBufferRenderTexture& target ) {

    if( target.id == 0 ){ return; }
 
    if( IsTextureValid ( target.albedo       )  )
      { rlUnloadTexture( target.albedo   .id ); }
    if( IsTextureValid ( target.depthness    )  )
      { rlUnloadTexture( target.depthness.id ); }

    rlUnloadFramebuffer( target.id );

}}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace rl {     
GBufferRenderTexture LoadGBuffer( int width, int height, int depth ) {
    
    GBufferRenderTexture target={0}; 
    target.id = rlLoadFramebuffer();

    if( target.id>0 ) {

        rlEnableFramebuffer(target.id);
        int format = image::FORMAT::FORMAT_UNCOMPRESSED_R8G8B8A8;

        target.width = width ;
        target.height= height;
        target.depth = depth ;

        target.albedo    = Texture2D({
            rlLoadTexture( NULL, width, height * depth, format, 1 ),
            width, height * depth, 1, format
        });

        target.depthness = Texture2D({
            rlLoadTextureDepth( width, height * depth, true),
            width, height * depth, 1, 19
        });

        rlActiveDrawBuffers(1);
        
        rlFramebufferAttach(target.id, target.albedo   .id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D   , 0);
        rlFramebufferAttach(target.id, target.depthness.id, RL_ATTACHMENT_DEPTH         , RL_ATTACHMENT_RENDERBUFFER, 0);

    if( rlFramebufferComplete(target.id) ){ 
        TRACELOG(LOG_INFO, "FBO: [ID %i] Framebuffer object created successfully", target.id);
    }

        rlDisableFramebuffer();
    } else { TRACELOG(LOG_WARNING, "FBO: Framebuffer object can not be created"); }

    return target;
}}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace rl { 
    
    Mesh GenMeshRaw( int pointCount, const Vector3* vertices, const Vector2* texCoords, const ushort* indices ) {
         
        Mesh mesh          = { 0 };
        mesh.triangleCount = pointCount-2; 
        mesh.vertexCount   = pointCount*1;

        if( vertices  ){ 
            mesh.vertices  = /*--*/ (float*)RL_MALLOC( mesh.vertexCount* 3* sizeof(float));
            memcpy( mesh.vertices , (float*)vertices,  mesh.vertexCount* 3* sizeof(float)); 
        } else { mesh.vertices = nullptr; }
        
        if( texCoords ){ 
            mesh.texcoords = /*--*/ (float*)RL_MALLOC( mesh.vertexCount* 2* sizeof(float));
            memcpy( mesh.texcoords, (float*)texCoords, mesh.vertexCount* 2* sizeof(float)); 
        } else { mesh.texcoords = nullptr; }

        if( indices   ){ 
            mesh.indices  = (ushort*)RL_MALLOC( mesh.triangleCount* 3* sizeof(ushort));
            memcpy( mesh.indices  , indices   , mesh.triangleCount* 3* sizeof(ushort)); 
        } else { mesh.indices = nullptr; }

        UploadMesh( &mesh, false ); 
        return mesh;

    }

    void DrawPolygon( ptr_t<Vector2> points, ptr_t<Vector2> coords, Color color ){

        if( points.size() >= 3 && points.size() == coords.size() ) {
        rlSetTexture( GetShapesTexture().id );
            
            Rectangle shapeRect = GetShapesTextureRectangle();
            Texture2D texShapes = GetShapesTexture();

        rlBegin(RL_QUADS);

            rlColor4ub( color.r, color.g, color.b, color.a );

        for( int i=1; i<points.size()-1; i++ ){

            rlTexCoord2f(coords[0].x*.5+.5  , coords[0].y*.5+.5 );
            rlVertex2f  (points[0].x        , points[0].y );

            rlTexCoord2f(coords[i].x*.5+.5  , coords[i].y*.5+.5 );
            rlVertex2f  (points[i].x        , points[i].y );

            rlTexCoord2f(coords[i+1].x*.5+.5, coords[i+1].y*.5+.5 );
            rlVertex2f  (points[i+1].x      , points[i+1].y );

            rlTexCoord2f(coords[0].x*.5+.5  , coords[0].y*.5+.5 );
            rlVertex2f  (points[0].x        , points[0].y );

        }

        rlEnd(); rlSetTexture(0);

    }}

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace rl { void BeginMode3D( camera_3D_t& cam ){
    
    rlDrawRenderBatchActive();        // Update and draw internal render batch
    rlMatrixMode(RL_PROJECTION);      // Switch to projection matrix
    rlPushMatrix(); rlLoadIdentity(); // Reset current matrix (projection)

    float aspect = GetRenderWidth() * 1.016f / GetRenderHeight();

    if ( cam.projection == CAMERA_PERSPECTIVE ) {

        double top   = cam.near * tan(cam.fovy * 0.5 * DEG2RAD);
        double right = aspect   * top;
        rlFrustum(-right, right, -top, top, cam.near, cam.far);

    } else {

        double top   = cam.fovy/2.0;
        double right = aspect * top;
        rlOrtho(-right, right, -top, top, cam.near, cam.far);

    }

    rlMatrixMode(RL_MODELVIEW);     // Switch back to modelview matrix
    rlLoadIdentity();               // Reset current matrix (modelview)

    Matrix matView = MatrixLookAt(cam.position, cam.target, cam.up);
    rlMultMatrixf(MatrixToFloat(matView));

    rlEnableDepthTest(); // Enable DEPTH_TEST for 3D

}}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/
