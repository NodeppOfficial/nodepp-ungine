#include <nodepp/nodepp.h>
#include <ungine/ungine.h>
#include <nodepp/json.h>
#include <nodepp/fs.h>

using namespace nodepp;

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { void main(){

    auto obj = json::parse( fs::read_file("./skeld.json").await().value() );
    auto bon = model::load( "boundaries.glb" );
    auto lmt = model::load( "limit.glb" );
    auto lin = model::load( "line.glb" );

    auto ppt = array_t<array_t<int>>();

    for( int x=0; x<bon.meshCount; x++ ){ 
    for( auto &y: ppt ){ if( y.some([=]( int z ){

        vec3_t bnd ({ .1, .1, .1, });
        box_t A = collision::box( bon.meshes[x] ); A.min += bnd; A.max -= bnd;
        box_t B = collision::box( bon.meshes[z] ); B.min += bnd; B.max -= bnd;
            
        return collision::is_hit( A, B );

    }) ){ y.push(x); goto NEXT; } } 

        ppt.push( array_t<int>({ x }) );

    NEXT:; }

    auto ppl = array_t<array_t<int>>( ppt.size() );

    for( int x=0; x<ppt.size()   ; x++ ){
    for( int y=0; y<lin.meshCount; y++ ){

        if( ppt[x].some([=]( int z ){

            box_t A = collision::box( bon.meshes[z] );
            box_t B = collision::box( lin.meshes[y] );

            return collision::is_hit( A, B );

        }) ){ ppl[x].push(y); }

    }}

    auto ptt = array_t<array_t<int>>( ppt.size() );

    for( int x=0; x<ppt.size()   ; x++ ){
    for( int y=0; y<lmt.meshCount; y++ ){

        if( ppt[x].some([=]( int z ){

            box_t A = collision::box( bon.meshes[z] );
            box_t B = collision::box( lmt.meshes[y] );

            return collision::is_hit( A, B );

        }) ){ ptt[x].push(y); }

    }}

    auto out = array_t<object_t>();
    auto tou = array_t<object_t>();

    for( int x=0; x<ppt.size(); x++ ){ array_t<object_t> A, B, C;

        for( auto y: ppt[x] ){
            box_t box = collision::box( bon.meshes[y] );
            A.push( array_t<object_t>({
                array_t<float>({ box.min.x, box.min.y, box.min.z }),
                array_t<float>({ box.max.x, box.max.y, box.max.z }),
            }) );
        }

        for( auto y: ptt[x] ){
            box_t box = collision::box( lmt.meshes[y] );
            B.push( array_t<object_t>({
                array_t<float>({ box.min.x, box.min.y, box.min.z }),
                array_t<float>({ box.max.x, box.max.y, box.max.z }),
            }) );
        }

        for( auto y: ppl[x] ){

            box_t box = collision::box( lin.meshes[y] );

            if( ( box.max.x - box.min.x ) < 
                ( box.max.z - box.min.z ) 
            ) { 
                float mean = ( box.max.x - box.min.x )/2;
                C.push( array_t<object_t>({
                    array_t<float>({ box.min.x + mean, box.min.z + mean }),
                    array_t<float>({ box.min.x + mean, box.max.z - mean })
                }) );
            } else {
                float mean = ( box.max.z - box.min.z )/2;
                C.push( array_t<object_t>({
                    array_t<float>({ box.min.x + mean, box.min.z + mean }),
                    array_t<float>({ box.max.x - mean, box.min.z + mean })
                }) );
            }

        }

        out.push( array_t<object_t>({ A, B }) );
        tou.push( array_t<object_t>({ A, C }) );

    }

    obj["line"]      = tou;
    obj["collision"] = out; obj.erase( "light" );
    fs::write_file( "out.json", json::format( obj ) ).await();
    process::exit();

} }

/*────────────────────────────────────────────────────────────────────────────*/

void onMain(){
    ungine::engine::start  ( 800, 600, "MyGame" ); // ( 600, 400, "MyGame" );
    ungine::engine::set_fps( 60 ); ungine::main();
}

/*────────────────────────────────────────────────────────────────────────────*/