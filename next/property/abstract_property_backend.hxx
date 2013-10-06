
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <functional>

namespace next
{
  /* virtual */ abstract_property_backend::~abstract_property_backend()
  {
  }

  template< typename Property >
  void abstract_property_backend::set( const typename Property::value_type& value )
  {
    untyped_set( static_cast< const typename Property::value_type* >( &value ) );
  }

  template< typename Property >
  void abstract_property_backend::listen( const std::function< void( const typename Property::value_type& ) >& f )
  {
    untyped_listen( &f );
  }

}