
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace next
{
  template< typename Property >
  property_backend< Property >::property_backend( Property& p )
    : property_( p )
  {
  }

  template< typename Property >
  /* virtual */ void property_backend< Property >::untyped_set( const void* value )
  {
    property_.set_from_backend( *static_cast< const typename Property::value_type* >( value ) );
  }

  template< typename Property >
  void property_backend< Property >::set( const typename Property::value_type& value )
  {
    property_.set_from_backend( value );
  }

  template< typename Property >
  /* virtual */ void property_backend< Property >::untyped_listen( const void* f )
  {
    notify_change_to_backend_ = *static_cast< const std::function< void( const typename Property::value_type& ) >* >( f );
  }

  template< typename Property >
  void property_backend< Property >::notify_change( const typename Property::value_type& value )
  {
    if( notify_change_to_backend_ )
    {
      notify_change_to_backend_( value );
    }
  }
}