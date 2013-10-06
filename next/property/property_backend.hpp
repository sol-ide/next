
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <functional>
#include <next/property/abstract_property_backend.hpp>

namespace next
{
  template< typename Property >
  class property_backend : public abstract_property_backend
  {
  public:
    property_backend( Property& p );

    void set( const typename Property::value_type& value );

  private:
    friend Property;

    virtual void untyped_set( const void* value );

    virtual void untyped_listen( const void* f );

    void notify_change( const typename Property::value_type& value );

  private:
    Property& property_;
    std::function< void( const typename Property::value_type& ) > notify_change_to_backend_;
  };
}

#include <next/property/property_backend.hxx>