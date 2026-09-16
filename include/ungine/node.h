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

namespace ungine { class node_t {
public:

    listener_t<string_t,any_t> onSignal;
    event_t<>                  onNext  ;
    event_t<>                  onU2D   ;
    event_t<>                  onUI    ;
    event_t<>                  on2D    ;
    event_t<>                  on3D    ;
    event_t<>                  onU3D   ;
    event_t<>                  onUUI   ;
    event_t<>                  onDraw  ;
    event_t<float>             onLoop  ;
    event_t<>                  onClose ;

protected:

    struct DONE {
        void *node=nullptr, *parent=nullptr, *root=nullptr;
        map_t<string_t,DONE> node_list;
    };

    struct NODE {
        array_t<ptr_t<task_t>>   task; object_t att;
        bool state = false; DONE node;
    };  ptr_t<NODE> obj;

    static void node_iterator( function_t<bool,node_t*> cb, node_t* root, bool deep ) {
        
        if( root == nullptr ){ return; } node_t* node = root;
        if( deep && !cb( type::cast<node_t>( node->obj->node.node ) ) )
          { return; }

        auto x = node->obj->node.node_list.raw().first(); while( x!=nullptr ){
        auto y = x->next; 

            if  ( x->data.second.node==nullptr ) /*----------------*/ { goto NEXT; }
            if  ( deep ){ node_iterator(cb,(node_t*)( x->data.second.node ),deep); }
            elif( !cb( type::cast<node_t>( x->data.second.node ) ) ) /**/ { break; }
             
        NEXT:; x=y; }

    }

    ptr_t<node_t> init() {
        auto self = type::bind( this );
        set_attribute( "name","root" ); 
        obj->node.root=&self;
        obj->node.node=&self; return self;
    }

public:

   ~node_t() noexcept { if( obj.count()>1 ){ return; } free(); }
    node_t() noexcept : obj( new NODE() ) { init(); }

    /*─······································································─*/

    node_t( function_t<void,ptr_t<node_t>> cb ) noexcept : obj( new NODE() ) {

        engine::get_lock()++; auto self = init(); obj->state = true;

        obj->task.push( engine::onClose.once([=](){ self->free(); }) );

        obj->task.push( engine::onLoop.add([=]( float delta ){
            if( !self->exists() ){ return -1; }
            /**/ self->onLoop.emit( delta );
        return 1; }) );

        obj->task.push( engine::onNext.add([=](){
            if( !self->exists() ){ return -1; }
            /**/ self->onNext.emit();
        return 1; }) );

        obj->task.push( engine::onDraw.add([=](){
            if( !self->exists() ){ return -1; }
            /**/ self->onDraw.emit(); 
        return 1; }) );

        process::add([=](){
            cb( self ); engine::get_lock()--; 
        return -1; });

    }

    /*─······································································─*/

    node_t* append_child( string_t name, const node_t& value ) const noexcept {
    do {

        if( !exists() ){ break; }
        if( has_child( name ) ){ remove_child( name ); }
        if( value.obj->node.parent!= nullptr ){ break; }

        value.set_attribute( "name", name );

        value.obj->node.parent   = obj->node.node ;
        value.obj->node.root     = obj->node.root ;
        obj->node.node_list[name]= value.obj->node;

        return get_child( name );
    
    } while(0); return nullptr; }

    node_t* append_child( const node_t& value ) const noexcept {
    return  append_child( string::to_string( value.obj->node.node ), value ); }

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
        if( has_child( name ) ){ get_child(name)->free(); }
    }

    void clear_children() const noexcept {
    for( auto &x: get_children() ){ x->free(); }}
    
    /*─······································································─*/

    ptr_t<render_queue_t> get_render_queue() const noexcept {
        auto view = get_viewport();
        if ( view==nullptr )  { return nullptr;  }
        auto que  = type::bind( render_queue_t() );

        get_root()->child_iterator([&]( node_t* node ){

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
    node_t*    root =get_root();
        return root==nullptr ? nullptr : root->get_viewport();
    }

    node_t* get_parent() const noexcept { 
        return type::cast<node_t>( obj->node.parent );
    }

    node_t* get_root() const noexcept { 
        return obj->node.root==nullptr ? (node_t*) this : (node_t*) obj->node.root;
    }

    /*─······································································─*/

    void node_iterator( function_t<void,node_t*> cb, bool deep=false ) const noexcept {
         node_iterator( [&]( node_t* node ){ cb(node); return true; }, type::cast<node_t>( obj->node.node ), deep );
    }

    void child_iterator( function_t<bool,node_t*> cb, bool deep=false ) const noexcept {
         node_iterator ( cb, type::cast<node_t>( obj->node.node ), deep );
    }

    /*─······································································─*/

    node_t* get_node() const noexcept { return type::cast<node_t>( obj->node.node ); }
    
    node_t* get_node( string_t node_path ) const noexcept {

        auto list = regex::split( path::normalize( node_path ), "/" );
        if ( list.empty() ){ return nullptr; } auto item = get_node();

        for( auto x: list ){
        if ( item == nullptr ){ break; }
        if ( x == ".." ){ item = item->get_parent( ); continue; }
        if ( x == "."  ){ item = item->get_node  ( ); continue; }
           /*----------*/ item = item->get_child (x);
        }

        return item; 

    }

    bool has_node( string_t node_path ) const noexcept { 
         return get_node( node_path ) != nullptr; 
    }

public:

    bool has_attribute   ( string_t name ) const noexcept { return obj->att.has(name); }

    void remove_attribute( string_t name ) const noexcept { obj->att.erase( name ); }

    /*─······································································─*/

    void set_attribute( string_t name, const char* value ) const noexcept {
        obj->att[ name ] = type::bind( string::to_string( value ) ); 
    }

    template< class T >
    void set_attribute( string_t name, T value ) const noexcept {
        obj->att[ name ] = type::bind( value );
    }

    void clear() const noexcept { obj->att.clear(); }

    /*─······································································─*/

    template< class T >
    ptr_t<T> get_attribute( string_t name ) const {
        if( !obj->att.has( name ) ){ return nullptr; }
        return obj->att[ name ].as<ptr_t<T>>();
    }

public:

    bool exists() const noexcept { return obj->state && obj->node.node!=nullptr; }
    void remove() const noexcept { free(); }
    void   free() const noexcept { if( !exists() ){ return; } obj->state= false; 

        if( get_parent()!= nullptr && has_attribute("name") ){ 
            auto name = get_attribute<string_t>    ("name");
            get_parent()->obj->node.node_list.erase(*name ); 
        }   clear_children(); onClose.emit(); 

        auto self = type::bind( this ); 
        
        engine::onFree.add([=](){

            /*-------------------*/ self->onNext .clear();
            self->onLoop  .clear(); self->onDraw .clear();
            self->onUI    .clear(); self->onUUI  .clear();
            self->on2D    .clear(); self->onU2D  .clear();
            self->on3D    .clear(); self->onU3D  .clear();
            self->onSignal.clear(); self->onClose.clear();
            
            engine::onClose.off( self->obj->task[0] ); 
            engine::onLoop .off( self->obj->task[1] ); 
            engine::onNext .off( self->obj->task[2] ); 
            engine::onDraw .off( self->obj->task[3] ); 
            
            self->obj->node.parent = nullptr;
            self->obj->node.node   = nullptr;
            self->obj->node.root   = nullptr;

        engine::get_lock()--; return -1; }); 
        engine::get_lock()++; 
    
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

#endif

/*────────────────────────────────────────────────────────────────────────────*/