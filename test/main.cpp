#include <nodepp/nodepp.h>
#include <ungine/ungine.h>

/*────────────────────────────────────────────────────────────────────────────*/

using namespace nodepp;

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { void main() {

    node::node_scene([=]( ptr_t<node_t> self ){

        self->on2D([=](){ rl::DrawFPS ( 10 , 10 ); });
        self->on3D([=](){ rl::DrawGrid( 100, 10 ); });
        self->get_viewport()->background = color::hex( "222222" );

        self->append_child( node::node_fly_camera_3D([=]( ptr_t<node_t> self ){
            auto pos = self->get_attribute<transform_3D_t>( "transform" );
            pos->position = vec3_t({ 0, 0, -20 });
            cursor::lock();
        }) );

        self->append_child( node::node_3D([=]( ptr_t<node_t> self ){

            self->append_child( node::node_cube([=]( ptr_t<node_t> self ){
                auto pos = self->get_attribute<transform_3D_t>( "transform" );
                auto shp = self->get_attribute<shape_3D_t>    ( "shape"     );

                auto shd = shader::load_material_shader( nullptr,
                NODEPP_STRINGIFY( material_t onMain( material_t mat, int layer ){
                       mat.result = vec4( 1., 0., 0., 1. );
                return mat; }));
                
                     pos->position = vec3_t({ 0, 3, 0 });
                     pos->scale    = vec3_t({ 1, 1, 1 });
                     shp->model.materials[0].shader = shd;

                self->onClose([=](){ shader::unload(shd); });
            }));

            self->append_child( node::node_cube([=]( ptr_t<node_t> self ){
                auto pos = self->get_attribute<transform_3D_t>( "transform" );
                     pos->position = vec3_t({  0, 0,  0 });
                     pos->scale    = vec3_t({ 30,.1, 30 });
            }));

        }));

    });

}}

/*────────────────────────────────────────────────────────────────────────────*/

void onMain(){
    ungine::engine::start  ( 800, 600, "MyGame" ); // ( 600, 400, "MyGame" );
    ungine::engine::set_fps( 60 ); ungine::main();
}

/*────────────────────────────────────────────────────────────────────────────*/