
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace next
{
  /* virtual */ abstract_property::~abstract_property()
  {
  }

  template< typename T >
  void abstract_property::set( const T& value )
  {
    untyped_set( static_cast< const T* >( &value ) );
  }

  template< typename T >
  const T& abstract_property::get() const
  {
    return *static_cast< const T* >( untyped_get() );
  }

  template< typename T >
  void abstract_property::listen( const std::function< void( const T& ) >& f )
  {
    untyped_listen( &f );
  }

  template< typename Property >
  property_backend< Property >& abstract_property::get_backend()
  {
    return *static_cast< property_backend< Property >* >( untyped_get_backend() );
  }
}