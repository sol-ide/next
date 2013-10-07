
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <mutex>

namespace next
{
  template< typename T >
  class property;

  template< typename Property >
  class property_backend;

  class abstract_property
  {
  public:
    virtual ~abstract_property();

  public:
    template< typename T >
    void set( const T& value );

    template< typename T >
    T get() const;

    template< typename T >
    void listen( const std::function< void( const T& ) >& f );

    template< typename Property >
    property_backend< Property >& get_backend();

  private:
    virtual void untyped_set( const void* value ) = 0;
    virtual const void* untyped_get() const = 0;
    virtual void* untyped_get_backend() = 0;
    virtual void untyped_listen( const void* f ) = 0;

  protected:
    mutable std::mutex property_mutex_;
  };
}

#include <next/property/abstract_property.hxx>