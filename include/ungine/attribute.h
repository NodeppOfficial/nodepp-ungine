/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_ATTRIBUTE
#define UNGINE_ATTRIBUTE

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { class attribute_t {
protected: object_t storage;
public:    attribute_t() noexcept {}

    /*─······································································─*/

    bool has_attribute   ( string_t name ) const noexcept { return storage.has(name); }
    void remove_attribute( string_t name ) const noexcept { storage.erase( name ); }

    /*─······································································─*/

    void set_attribute( string_t name, const char* value ) const noexcept {
         storage[ name ] = type::bind( string::to_string( value ) ); 
    }

    template< class T >
    void set_attribute( string_t name, T value ) const noexcept {
         storage[ name ] = type::bind( value );
    }

    void clear() noexcept { storage.clear(); }

    /*─······································································─*/

    template< class T >
    ptr_t<T> get_attribute( string_t name ) const {
        if( !storage.has( name ) ){ return nullptr; }
        return storage[ name ].as<ptr_t<T>>();
    }

};}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/