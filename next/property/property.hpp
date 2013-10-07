
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <next/property/property_backend.hpp>
#include <next/property/abstract_property.hpp>

#include <deque>

namespace next
{
  template< typename T >
  class property : public abstract_property
  {
  public:
    typedef T value_type;

    property( const T& value );

    property();

  private:
    template< typename Properties > 
    friend class property_backend;

    virtual void untyped_set( const void* value );
    virtual const void* untyped_get() const;
    virtual void untyped_listen( const void* f );
    virtual void* untyped_get_backend();

  private:
    void set_from_backend( const T& value );

  private:
    T value_;
    next::property_backend< property< T > > backend_;

    typedef std::deque< std::function< void( const T& ) > > notify_list_type;
    notify_list_type notify_change_to_frontend_;
  };
}


#include <next/property/property.hxx>