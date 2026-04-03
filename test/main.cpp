#include <nodepp/nodepp.h>
#include <ungine/ungine.h>
#include <nodepp/bind.h>

/*────────────────────────────────────────────────────────────────────────────*/

using namespace nodepp;

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { void main() {

    node::node_scene([=]( ptr_t<node_t> self ){

        self->on2D([=](){ rl::DrawFPS ( 10 , 10 ); });
        self->get_viewport()->background = color::hex( "222222" );

        self->append_child( node::node_light([=]( ptr_t<node_t> self ){

            auto pos = self->get_attribute<transform_3D_t>( "transform" );
            auto lgh = self->get_attribute<light_t>( "light" );

            pos->position = vec3_t({ 0, 15, 0 });

            lgh->mode    = light::MODE::LIGHT_MODE_AMBIENT;
            lgh->energy  = 10.f;
            lgh->color   = color_t({ 255, 255, 255, 255 }); 

        }));

        self->append_child( node::node_3D([=]( ptr_t<node_t> self ){
            auto pos = self->get_attribute<transform_3D_t>( "transform" );
            auto mdl = model::load( "./assets/skeld.glb" );

            self->onClose([=]( node_t* ){ model::unload( mdl ); });
            self->onU3D  ([=](){
                mat_t transform = math::matrix::from_transform_3D( *pos );
                rl::DrawMesh( mdl.meshes[0], mdl.materials[1], transform );
            });

            self->append_child( node::node_light( [=]( ptr_t<node_t> self ){

                auto pos = self->get_attribute<transform_3D_t>( "transform" );
                auto lgh = self->get_attribute<light_t>( "light" );
                auto bdb = rl::GetMeshBoundingBox( mdl.meshes[0] );

                lgh->mode    = light::MODE::LIGHT_MODE_AREA;
                lgh->energy  = 30.0f;
                lgh->wrap    = 3 * PI / 4; 
                pos->position= bdb.min * 3;
                pos->rotation= vec3_t({ PI, 0, 0 });
                pos->scale   = vec3_t({ 100, 80, 100 });

                self->on3D([=](){
                    rl::DrawSphere( pos->translate.position, 1, rl::RED );
                });

            }));

            pos->scale = vec3_t({ 3, 3, 3 });
        }));

        self->append_child( node::node_3D([=]( ptr_t<node_t> self ){

            self->append_child( node::node_camera_3D([=]( ptr_t<node_t> self ){

                auto pos = self->get_attribute<transform_3D_t>( "transform" );
                pos->position = vec3_t({ 0, 50, -3 });
                pos->rotation = vec3_t({ PI / 2, 0, 0 });

            }) );

        self->append_child( node::node_model( "./assets/astronaut.glb", [=]( ptr_t<node_t> self ){

            auto ppr = self->get_parent()->get_attribute<transform_3D_t>( "transform" );
            auto pos = self->get_attribute<transform_3D_t>( "transform" );
            auto shp = self->get_attribute<shape_3D_t>( "shape" );
            auto dir = ptr_t<vec2_t>( 0UL, vec2_t({ 0, 0 }) );

            pos->scale    = vec3_t({ 10, 10, 10 });
            pos->position = vec3_t({ 0, 0, 0 });
            ptr_t<float> speed ( 0UL, 15.f );

            shp->animation.mode  = animation::MODE::ANIMATION_MODE_PLAY;
            shp->animation.index = 2;
            shp->animation.step  = 10;

            self->append_child( node::node_light( [=]( ptr_t<node_t> self ){

                auto pos = self->get_attribute<transform_3D_t>( "transform" );
                auto lgh = self->get_attribute<light_t>( "light" );

                lgh->mode    = light::MODE::LIGHT_MODE_SPOT;
                lgh->energy  = 100.f;
                pos->rotation= vec3_t({ -PI/2, 0, 0 });
                pos->position= vec3_t({ 0, 1, 0 });
                lgh->color   = color_t({ 255, 255, 255, 255 }); 

                self->on3D([=](){
                    rl::DrawSphere( pos->translate.position, 1, lgh->color );
                });

            }));

            self->onLoop([=]( float delta ){
               *dir = vec2_t({ 0, 0 }); *speed = 0.; shp->animation.index = 0;

                if( key::is_down( 'W' ) ){
                    dir->y = 1; shp->animation.index = 2;
                } elif( key::is_down( 'S' ) ) {
                    dir->y =-1; shp->animation.index = 2;
                }

                if( key::is_down( 'A' ) ){
                    dir->x = 1; shp->animation.index = 2;
                } elif( key::is_down( 'D' ) ) {
                    dir->x =-1; shp->animation.index = 2;
                }

                if( key::is_down( key::BUTTON::KEY_LEFT_SHIFT ) ){ do {
                if( shp->animation.index == 0 ){ break; }
                    shp->animation.index =  1;
                } while(0); *speed = 45.; } else { *speed = 30.; }

                if( dir->x==0 && dir->y==0 ){ return; }

                pos->rotation.y = atan2f( dir->y, -dir->x ) - PI / 2;
                ppr->position.x+= dir->x * delta ** speed;
                ppr->position.z+= dir->y * delta ** speed;

            });

        }) );

        }) );

    });

}}

/*────────────────────────────────────────────────────────────────────────────*/

void onMain(){
    ungine::engine::start  ( 800, 600, "MyGame" ); // ( 600, 400, "MyGame" );
    ungine::engine::set_fps( 60 ); ungine::main();
}

/*────────────────────────────────────────────────────────────────────────────*/