#include <nodepp/nodepp.h>
#include <nodepp/cluster.h>
#include <ungine/ungine.h>

/*────────────────────────────────────────────────────────────────────────────*/

using namespace nodepp;

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { void main() {

    node::node_scene([=]( ref_t<node_t> self ){

        auto model = model_t( rl::GenMeshCube( 1, 1, 1 ) );
        auto img   = texture_t( "./texture.jpg" );
        auto pos   = transform_3D_t();
        auto shd   = shader_t();

        self->get_viewport()->mode |= visibility::MODE::VISIBILITY_MODE_VR;
        pos.translate.scale *= 10;

        shd.set_fragment_shader( kernel::fs_default_kernel() );
        shd.set_vertex_shader  ( kernel::vs_default_kernel() );
        
        if( shd.compile() ){
            shd.append_uniform( "img", img );
            shd.append_uniform( "slf", pos.translate.position );
            shd.append_uniform( "lgh", vec3_t({ 10, 10, 10 }) );
            model->materials[0].shader = shd.get( /*-------*/ );
        }

        self->append_child( node::node_fly_camera_3D( nullptr ) );

        self->onLoop([=]( float /*unused*/ ){
            shd.append_uniform( "cam", self->get_root_viewport()->camera3D->position );
        });

        self->on3D([=](){ shd.next(); model.draw ( pos, rl::WHITE ); });
        self->on2D([=](){ rl::DrawFPS ( 10 , 10 ); });
        self->on3D([=](){ rl::DrawGrid( 100, 10 ); });

        cursor::lock();

    });

}}

/*────────────────────────────────────────────────────────────────────────────*/

void onMain(){
    ungine::engine::start  ( 800, 600, "MyGame" );
    ungine::engine::set_fps( 60 ); ungine::main();
}

/*────────────────────────────────────────────────────────────────────────────*/