/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_NODE
#define UNGINE_NODE

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace node { struct NODE_TREE {
   void *node=nullptr, *parent=nullptr;
   map_t<string_t,NODE_TREE> node_list;
}; }}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { class node_t : public global_t {
public:

    event_t<node_t*,any_t> onCollision;
    event_t<>              onNext  ;
    event_t<>              onU2D   ;
    event_t<>              onUI    ;
    event_t<>              on2D    ;
    event_t<>              on3D    ;
    event_t<>              onU3D   ;
    event_t<>              onUUI   ;
    event_t<>              onDraw  ;
    event_t<float>         onLoop  ;
    event_t<node_t*>       onClose ;

protected:

    using DONE = node::NODE_TREE; struct NODE {
        bool state = false; DONE node;
        ptr_t<task_t> task;
    };  ptr_t<NODE> obj;

    static void node_iterator( function_t<bool,node_t*> cb, node_t* root, bool deep ) {
        
        if( root == nullptr ){ return; } node_t* node = root;
        if( deep && !cb( type::cast<node_t>( node->obj->node.node ) ) )
          { return; }

        auto x = node->obj->node.node_list.raw().first(); while( x!=nullptr ){
        auto y = x->next; 

            if  ( x->data.second.node==nullptr ) /*--------------------*/ { goto NEXT; }
            if  ( deep ){ node_iterator( cb, (node_t*)( x->data.second.node ), deep ); }
            elif( !cb( type::cast<node_t>( x->data.second.node ) ) ) /**/ { break; }
             
        NEXT:; x=y; }

    }

    void remove_from_parent() const noexcept {
         node_t* prnt = get_parent(); if( prnt==nullptr ){ return; }
         prnt->remove_child( *get_attribute<string_t>("name") );
    }

    ptr_t<node_t> init() {
        auto self = type::bind( this );
        self->set_attribute( "name", "root" ); 
        self->obj->node.node=&self; return self;
    }

public:

   ~node_t() noexcept { if( obj.count()>1 ){ return; } free(); }
    node_t() noexcept : global_t(), obj ( new NODE ) { init(); }

    /*─······································································─*/

    node_t( function_t<void,ptr_t<node_t>> cb ) noexcept : global_t(), obj( new NODE ) {
        engine::locker.lock(); auto self = init(); obj->state = true;
        obj->task = engine::onClose.once([=](){ self->free(); });

        engine::onLoop.add([=]( float delta ){
            if( !self->exists() ){ return -1; }
            self->onLoop.emit( delta );
        return 1; });

        engine::onNext.add([=]( /**/ ){
            if( !self->exists() ){ return -1; }
            self->onNext.emit();
        return 1; });

        engine::onDraw.add([=]( /**/ ){
            if( !self->exists() ){ return -1; }
            self->onDraw.emit(); 
        return 1; });

        process::add([=](){
            cb( self ); engine::locker.unlock(); 
        return -1; });

    }

    /*─······································································─*/

    node_t* append_child( string_t name, const node_t& value ) const noexcept {

        if( !exists() ) /*--------*/ { return nullptr; }
        if( has_child( name ) ){ remove_child( name ); }

        value.set_attribute( "name", name );

        value.obj->node.parent   = obj->node. node;
        obj->node.node_list[name]= value.obj->node;

        return get_child( name );
    }

    node_t* append_child( const node_t& value ) const noexcept {
        return append_child( encoder::key::generate(12), value );
    }

    bool has_child( string_t name ) const noexcept { 
        if( !exists() ) /*------------*/ { return false; }
        if( obj->node.node_list.empty() ){ return false; }
        return obj->node.node_list.has( name ); 
    }

    /*─······································································─*/

    ulong count_children() const noexcept { return obj->node.node_list.size(); }

    ptr_t<node_t*> get_children() const noexcept { ulong w=0;
    ptr_t<node_t*> out( count_children() );

        auto x = obj->node.node_list.raw().first(); while(x!=nullptr){
        auto z = type::cast<node_t>( x->data.second.node );
        auto y = x->next; out[w]=z; x=y; ++w; }

    return out; }

    node_t* get_child( string_t name ) const noexcept {
        if( !has_child( name ) ) /*----------*/ { return nullptr; }
        return type::cast<node_t>( obj->node.node_list[name].node );
    }

    void remove_child( string_t name ) const noexcept {
        node_t* child = get_child(name); 
        if( child==nullptr ) /*-------------------*/ { return; }
        if( child->obj->node.parent!=obj->node.node ){ return; }
        obj->node.node_list.erase(name); child->free(); 
    }
    
    /*─······································································─*/

    ptr_t<render_queue_t> get_render_queue() const noexcept {
        auto view = get_viewport();
        if ( view==nullptr )  { return nullptr;  }
        auto que  = type::bind( render_queue_t() );

        get_root()->child_iterator([=]( node_t* node ){

            if( node->has_attribute /*---------------*/ ("visibility") ){
            auto vis = node->get_attribute<visibility_t>("visibility");
            if(  vis->mode == 0x00 ) /*---*/ { return false; }
            if(( vis->mask & view->mask )==0){ return false; }}

        //  if( node->has_attribute( "viewport" ) ){ return; }

            if( !node->on3D .empty() ){ que->event3D .push( node->on3D  ); }
            if( !node->on2D .empty() ){ que->event2D .push( node->on2D  ); }
            if( !node->onUI .empty() ){ que->eventUI .push( node->onUI  ); }
            if( !node->onU2D.empty() ){ que->eventU2D.push( node->onU2D ); }
            if( !node->onU3D.empty() ){ que->eventU3D.push( node->onU3D ); }
            if( !node->onUUI.empty() ){ que->eventUUI.push( node->onUUI ); }

        return true; }, true ); return que;

    }

    viewport_t* get_viewport() const noexcept {
        node_t* root = type::cast<node_t>( obj->node.node ); 

        do { if ( root->get_parent()==nullptr ){ break; }
        if ( root->has_attribute( "viewport" )){ break; }
             root=root->get_parent(); 
        } while ( root->get_parent()!=nullptr );

        if (!root->has_attribute( "viewport" )){ return nullptr; }
        return &root->get_attribute<viewport_t>( "viewport" );
    }

    viewport_t* get_root_viewport() const noexcept {
        node_t* root =get_root(); /*-------------------------*/
        return  root==nullptr ? nullptr : root->get_viewport();
    }

    node_t* get_parent() const noexcept { 
        node_t* parent =type::cast<node_t>( obj->node.parent );
        return  parent==nullptr? nullptr: parent; 
    }

    node_t* get_root() const noexcept { 
    node_t* root = type::cast<node_t>( obj->node.node ); 

        do { if ( root->get_parent()==nullptr ){ break; }
             root=root->get_parent(); 
        } while ( root->get_parent()!=nullptr );

        return root;
    }

    /*─······································································─*/

    void node_iterator( function_t<void,node_t*> cb, bool deep=false ) const noexcept {
         node_iterator( [=]( node_t* node ){ cb(node); return true; }, type::cast<node_t>( obj->node.node ), deep );
    }

    void child_iterator( function_t<bool,node_t*> cb, bool deep=false ) const noexcept {
         node_iterator ( cb, type::cast<node_t>( obj->node.node ), deep );
    }

    /*─······································································─*/

    node_t* get_node( string_t node_path ) const noexcept {

        auto list = regex::split( path::normalize( node_path ), "/" );
        if( list.empty()   ) /*-*/ { return nullptr; }
        if( !has_child( list[0] ) ){ return nullptr; }
        if( list.size()==1 ) /*-*/ { return get_child( list[0] ); }

        return get_child( list[0] )->get_node( list.slice(1).join("/") );

    }

    node_t* get_node() const noexcept {  
        return type::cast<node_t>( obj->node.node );
    }

    /*─······································································─*/

    bool exists() const noexcept { return obj->state && obj->node.node!=nullptr; }

    /*─······································································─*/

    void free() const noexcept {

        if ( !exists() ) /*---------*/ { return; } obj->state= false ; 
        for( auto x: get_children()   ){ x->obj->node.parent =nullptr; }
        if ( !process::should_close() ){ remove_from_parent(); }

        onCollision.clear(); onNext.clear();
        onLoop     .clear(); onDraw.clear();
        onUI       .clear(); onUUI .clear();
        on2D       .clear(); onU2D .clear();
        on3D       .clear(); onU3D .clear();
        
        engine::onClose.off( obj->task );
        onClose.emit( (node_t*) this );
        
        obj->node.parent = nullptr;
        obj->node.node   = nullptr;

    }

};}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace node { node_t node_UI( function_t<void,ptr_t<node_t>> clb ){
return node_t([=]( ptr_t<node_t> self ){

    auto tmp /**/ = visibility_t();
         tmp.mode = visibility::MODE::VISIBILITY_MODE_ON ;
         tmp.mask = visibility::MASK::VISIBILITY_MASK_ALL;

    self->set_attribute( "transform" , transform_2D_t() );
    self->set_attribute( "visibility", tmp );

    self->onLoop([=]( float delta ){ 

        auto tr = self->get_attribute<transform_2D_t>( "transform" );
        auto pr = self->get_parent();

    if ( pr != nullptr ){
    auto pt = pr->get_attribute<transform_2D_t>( "transform" );
    if ( pt == nullptr ){ goto DEFAULT; }

        auto sc = pt->translate.scale    * tr->scale   ;
        auto rt = pt->translate.rotation + tr->rotation;

        auto ps = pt->translate.position 
        /*---*/ + rl::Vector2Rotate( tr->position, pt->translate.rotation );

        tr->translate.position = ps;
        tr->translate.rotation = rt;
        tr->translate.scale    = tr->scale; // sc

    } else { DEFAULT:;

        tr->translate.scale    = tr->scale;
        tr->translate.rotation = tr->rotation;
        tr->translate.position = tr->position;

    }});

clb(self); }); }}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace node { node_t node_2D( function_t<void,ptr_t<node_t>> clb ){
return node_t([=]( ptr_t<node_t> self ){

    auto tmp /**/ = visibility_t();
         tmp.mode = visibility::MODE::VISIBILITY_MODE_ON ;
         tmp.mask = visibility::MASK::VISIBILITY_MASK_ALL;

    self->set_attribute( "transform" , transform_2D_t() );
    self->set_attribute( "visibility", tmp );

    self->onLoop([=]( float ){ 

        auto tr = self->get_attribute<transform_2D_t>( "transform" );
        auto pr = self->get_parent();

    if ( pr != nullptr ){
    auto pt = pr->get_attribute<transform_2D_t>( "transform" );
    if ( pt == nullptr ){ goto DEFAULT; }

        auto sc = pt->translate.scale    * tr->scale   ;
        auto rt = pt->translate.rotation + tr->rotation;

        auto ps = pt->translate.position 
        /*---*/ + rl::Vector2Rotate( tr->position, pt->translate.rotation );

        tr->translate.position = ps;
        tr->translate.rotation = rt;
        tr->translate.scale    = tr->scale; // sc

    } else { DEFAULT:;

        tr->translate.scale    = tr->scale;
        tr->translate.rotation = tr->rotation;
        tr->translate.position = tr->position;

    }});

clb(self); }); }}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace node { node_t node_3D( function_t<void,ptr_t<node_t>> clb ){
return node_t([=]( ptr_t<node_t> self ){

    auto tmp /**/ = visibility_t();
         tmp.mode = visibility::MODE::VISIBILITY_MODE_ON ;
         tmp.mask = visibility::MASK::VISIBILITY_MASK_ALL;

    self->set_attribute( "transform" , transform_3D_t() );
    self->set_attribute( "visibility", tmp );

    self->onLoop([=]( float delta ){ 

        auto tr = self->get_attribute<transform_3D_t>( "transform" );
        auto pr = self->get_parent();

    if ( pr != nullptr ){
    auto pt = pr->get_attribute<transform_3D_t>( "transform" );
    if ( pt == nullptr ){ goto DEFAULT; }

        tr->translate.position = math::vec3      ::rotation  ( tr->position, 
        /*------------------*/   math::quaternion::from_euler( pt->translate.rotation )) 
        /*------------------*/ + pt->translate.position;

        tr->translate.rotation = math::quaternion::to_euler  ( math::quaternion::multiply(
        /*------------------*/   math::quaternion::from_euler( pt->translate.rotation ) 
        /*------------------*/ , math::quaternion::from_euler( tr->rotation )));

        tr->translate.scale    = tr->scale;

    } else { DEFAULT:;

        tr->translate.position = tr->position;
        tr->translate.rotation = tr->rotation;
        tr->translate.scale    = tr->scale;

    }});

clb(self); }); }}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace node { 

    node_t node_light( light_t light, function_t<void,ptr_t<node_t>> clb ){
    return node_3D([=]( ptr_t<node_t> self ){ 

        self->set_attribute( "light", light );
        auto vpt = self->get_viewport();

    if( vpt==nullptr ){ return; }

        vpt->queue_light.push( &self ); auto ID=vpt->queue_light.last();
        self->onClose([=]( node_t* ){ vpt->queue_light.erase(ID); });

        auto pos = self->get_attribute<transform_3D_t>( "transform" );
        auto lgh = self->get_attribute<light_t>( "light" );
        auto mtx = self->get_root_viewport()->matrix;

        self->onNext([=](){

            int  x  = (int) matrix::get_pixel_float( mtx, 5, 0, 3 );
            auto lgt= *lgh; lgt.color.a = (uchar) lgt.mode;
            auto dir= math::vec3::rotation( 
                vec3_t({ 0, 1, 0 }),
            math::quaternion::from_euler( 
                pos->translate.rotation 
            ));

        if( lgt.mode == light::MODE::LIGHT_MODE_NONE ){ return; }

            matrix::set_pixel_float( mtx, x, 1, 0, pos->translate.position.x );
            matrix::set_pixel_float( mtx, x, 1, 1, pos->translate.position.y );
            matrix::set_pixel_float( mtx, x, 1, 2, pos->translate.position.z );
            matrix::set_pixel_color( mtx, x, 1, 3, lgt.color );

            matrix::set_pixel_float( mtx, x, 2, 0, pos->translate.scale.x );
            matrix::set_pixel_float( mtx, x, 2, 1, pos->translate.scale.y );
            matrix::set_pixel_float( mtx, x, 2, 2, pos->translate.scale.z );
            matrix::set_pixel_float( mtx, x, 2, 3, lgt.wrap );

            matrix::set_pixel_float( mtx, x, 3, 0, dir.x );
            matrix::set_pixel_float( mtx, x, 3, 1, dir.y );
            matrix::set_pixel_float( mtx, x, 3, 2, dir.z );
            matrix::set_pixel_float( mtx, x, 3, 3, lgt.energy );

            matrix::set_pixel_float( mtx, 5, 0, 3, (float)( x + 1 ) );
            
        });

    clb( self ); }); }


    node_t node_light( function_t<void,ptr_t<node_t>> clb ){
    return node_light( light_t(), clb ); }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace node { 

    node_t node_audio_3D( const music_t music, function_t<void,ptr_t<node_t>> clb ){
    return node_3D([=]( ptr_t<node_t> self ){

        self->set_attribute( "music", music );
        auto pos = self->get_attribute<transform_3D_t>("transform");
        auto snd = self->get_attribute<music_t>( "music" );
        auto vpt = self->get_viewport();

        self->onClose([=]( node_t* ){ music::unload( music ); });
        self->onNext.add( coroutine::add( COROUTINE(){
        coBegin

            if( music::is_valid( *snd ) ) { coEnd } while( true ){ 
                coWait( !music::is_playing( *snd ) ); 
            do {

                if( fabsf( music::seek( *snd ) - music::size( *snd ) ) < 0.1f &&
                    snd->looping && music::is_playing( *snd )
                ) { music::stop( *snd ); music::tell( *snd, 0.f ); break; }
                
                music::update( *snd );

            } while(0); coNext; }

        coFinish
        }));

        self->onLoop.add([=]( float delta ){
            if( vpt == nullptr ) /**/ { return -1; }
            if( vpt->camera3D.null() ){ return -1; }
            if( pos == nullptr ) /**/ { return -1; }

            auto cam_pos = vpt->camera3D->position;
            auto cam_tar = vpt->camera3D->target  ;

            float dist = math::distance( cam_pos, pos->position );
            float max_dist = 50.0f;
            float volume = 1.0f - ( dist / max_dist );
            if  ( volume < 0.0f ) { volume = 0.0f; }

            music::set_volume( *snd, volume * volume );

            vec3_t relative = Vector3Subtract ( pos->position, cam_pos );
            vec3_t forward  = Vector3Normalize( Vector3Subtract( cam_tar, cam_pos ) );
            vec3_t right    = Vector3Normalize( Vector3CrossProduct( forward, {0,1,0} ) );

            float pan = Vector3DotProduct( Vector3Normalize(relative), right );
            music::set_pan( *snd, ( pan + 1.0f ) / 2.0f );

        return 1; });

    clb( self ); }); } 

    /*─······································································─*/

    node_t node_audio( const music_t music, function_t<void,ptr_t<node_t>> clb ){
    return node_audio_3D( music, [=]( ptr_t<node_t> self ){
           self->onLoop.clear();
           self->remove_attribute( "transform" ); 
    clb( self ); }); } 

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/
