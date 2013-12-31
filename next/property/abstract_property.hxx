
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace next
{
  template< typename T >
  void abstract_property::set( const T& value )
  {
    std::unique_lock< std::mutex > lock( property_mutex_ );
    untyped_set( static_cast< const T* >( &value ) );
  }

  template< typename T >
  T abstract_property::get() const
  {
    std::unique_lock< std::mutex > lock( property_mutex_ );
    return *static_cast< const T* >( untyped_get() );
  }

  template< typename T >
  void abstract_property::listen( const std::function< void( const T& ) >& f )
  {
    std::unique_lock< std::mutex > lock( property_mutex_ );
    untyped_listen( &f );
  }

  template< typename Property >
  property_backend< Property >& abstract_property::get_backend()
  {
    return *static_cast< property_backend< Property >* >( untyped_get_backend() );
  }
}
