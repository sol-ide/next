
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <functional>

namespace next
{
  class abstract_property_backend
  {
  public:

    virtual ~abstract_property_backend();

    template< typename Property >
    void set( const typename Property::value_type& value );

    template< typename Property >
    void listen( const std::function< void( const typename Property::value_type& ) >& f );

  private:
    virtual void untyped_set( const void* value ) = 0;
    virtual void untyped_listen( const void* f ) = 0;
  };


}

#include <next/property/abstract_property_backend.hxx>