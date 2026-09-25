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

namespace ungine { class node_t : public attribute_t {
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

    struct NODE { bool exists=false; 
        uchar_64 self, root, parent;
        array_t<ptr_t<task_t>>   task;
        map_t<string_t,uchar_64> node;
    };  ptr_t<NODE> obj;

    handler_t<node_t>& node_handler() const noexcept { static handler_t<node_t> out; return out; }

    void node_iterator( function_t<bool,node_t*> cb, node_t* root, bool deep ) const noexcept {

        if( !root ) /*------*/ { return; }
        if( deep && !cb(root) ){ return; } auto mem = node_handler();

        auto x = root->obj->node.raw().first(); while( x!=nullptr ){
        auto y = x->next; auto node = &mem.read( x->data.second );

            if  ( deep ){ node_iterator( cb, node, deep ); }
            elif( !cb( node ) ){ break; }
             
        x=y; }

    }

public:

   ~node_t() noexcept { if( obj.count()>1 ){ return; } free(); }
    node_t() noexcept : obj( new NODE() ), attribute_t() {}

    /*─······································································─*/

    node_t( function_t<void,ptr_t<node_t>> cb ) noexcept : obj( new NODE() ), attribute_t() {

        auto mem  = node_handler(); auto hdl= mem.create(); obj->exists=true;
        mem.update( hdl, *this ); obj->self = hdl; obj->root = hdl;
        auto self = mem.read( obj->self );

        engine::onConstructor.once([=](){ cb( self ); });

        obj->task.push( engine::onLoop.add([=]( float delta ){
        do { if( !self->exists() ){ break; }
             self->onLoop.emit( delta ); return 1;
        } while(0); return -1; }) );

        obj->task.push( engine::onNext.add([=](){
        do { if( !self->exists() ){ break; }
             self->onNext.emit(); return 1;
        } while(0); return -1; }) );

        obj->task.push( engine::onDraw.add([=](){
        do { if( !self->exists() ){ break; }
             self->onDraw.emit(); return 1;
        } while(0); return -1; }) );

        obj->task.push( engine::onClose.add([=](){ 
        do { if( !self->exists() ){ break; }
             self->free(); /*return 1*/
        } while(0); return -1; }) );

    }

    /*─······································································─*/

    node_t* append_child( string_t name, const node_t& value ) const noexcept {
    do{ if( !exists()         ){ value.free(); break; }
        if( !value.exists()   ){ /*---------*/ break; }
        if( value.obj->parent ){ /*---------*/ break; }
        if( has_child( name ) ){ remove_child (name); }

        value.set_attribute( "name", name );

        value.obj->parent = obj->self;
        value.obj->root   = obj->root;
        obj->node[ name ] = value.obj->self;

    return get_child( name ); } while(0); return nullptr; }

    bool has_child( string_t name ) const noexcept { 
         return exists() ? obj->node.has( name ) : false; 
    }

    node_t* append_child( const node_t& value ) const noexcept {
    return  append_child( string::to_string( value.obj->self ), value ); }

    /*─······································································─*/

    ptr_t<node_t*> get_children() const noexcept { do {

        if( obj->node.empty() ){ break; }
        
        ptr_t<node_t*> out( obj->node.size() ); ulong x=0;
        obj->node.raw().map([&]( pair_t<string_t,uchar_64> item ){
            out[x] = &node_handler().read( item.second ); 
        x++; });

    return out; } while(0); return nullptr; }
    
    /*─······································································─*/

    void remove_child( string_t name ) const noexcept { do {
        if( !has_child( name ) ){ break; } 
        get_child( name )->free(); 
    } while(0); }

    node_t* get_child( string_t name ) const noexcept {
    do{ if( !has_child( name ) ){ break; }
        return &node_handler().read( obj->node[name] );
    } while(0); return nullptr; }

    void clear_children() const noexcept {
    for( auto &x: get_children() ){ x->free(); }}
    
    /*─······································································─*/

    ptr_t<render_queue_t> get_render_queue() const noexcept { do {
        auto view = get_root_viewport(); if( !view ){ break; }
        auto que  = type::bind( render_queue_t() );

        get_root()->child_iterator([&]( node_t* node ){

            if( node->has_attribute /*---------------*/ ("visibility") ){
            auto vis = node->get_attribute<visibility_t>("visibility") ;
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

    } while(0); return nullptr; }

    /*─······································································─*/

    viewport_t* get_root_viewport() const noexcept { return get_root()->get_viewport(); }
    viewport_t* get_viewport     () const noexcept { do {
        
        node_t* root = get_node(); while( root ) {
        if( root->has_attribute( "viewport" )){ break; }
            root=root->get_parent(); 
        } if (!root ) { break; }

    return &root->get_attribute<viewport_t>( "viewport" );
    } while(0); return nullptr; }

    /*─······································································─*/

    node_t* get_node  () const noexcept { return (node_t*) this; }
    node_t* get_root  () const noexcept { return &node_handler().read( obj->root   ); }
    node_t* get_parent() const noexcept { return &node_handler().read( obj->parent ); }

    /*─······································································─*/

    void node_iterator( function_t<void,node_t*> cb, bool deep=false ) const noexcept {
         node_iterator( [&]( node_t* node ){ cb(node); return true; }, get_node(), deep );
    }

    void child_iterator( function_t<bool,node_t*> cb, bool deep=false ) const noexcept {
         node_iterator ( cb, get_node(), deep );
    }

    /*─······································································─*/

    bool    has_node( string_t node_path ) const noexcept { return get_node( node_path ); }
    node_t* get_node( string_t node_path ) const noexcept { do {

        auto list = regex::split( path::normalize( node_path ), "/" );
        if ( list.empty() ){ break; } auto item = get_node();

        for( auto x: list    ){
        if ( item == nullptr ){ break; }
        if ( x == ".." ){ item = item->get_parent( ); continue; }
        if ( x == "."  ){ item = item->get_node  ( ); continue; }
           /*----------*/ item = item->get_child (x);
        }

    return item; } while(0); return nullptr; }

public:

    bool exists() const noexcept { return obj->exists==true; }
    void remove() const noexcept { free(); }
    void   free() const noexcept { if( !exists() ){ return; }

        auto self   = type::bind( this ); obj->exists=false; 
        clear_children(); onClose.emit(); 

        if( get_parent() && has_attribute("name") ){ 
            auto name = get_attribute<string_t>("name");
            get_parent()->obj->node.erase( name[0] ); 
        }

        engine::onDestructor.once([=](){

            self->node_handler().remove( self->obj->self );

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
            
            self->obj->parent = 0ULL;
            self->obj->self   = 0ULL;
            self->obj->root   = 0ULL;

        });
    
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