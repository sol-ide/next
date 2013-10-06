
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <algorithm>
#include <functional>


namespace next
{
  template< typename T >
  property< T >::property( const T& value )
    : backend_( *this )
    , value_( value )
  {
  }

  template< typename T >
  property< T >::property()
    : backend_( *this )
  {
  }

  template< typename T >
  /* virtual */ void property< T >::untyped_set( const void* value )
  {
    backend_.notify_change( *static_cast< const T* >( value ) );
  }

  template< typename T >
  /* virtual */ const void* property< T >::untyped_get() const
  {
    return &value_;
  }

  template< typename T >
  /* virtual */ void property< T >::untyped_listen( const void* f )
  {
    notify_change_to_frontend_.push_back( *static_cast< const std::function< void( const T& ) >* >( f ) );
  }

  template< typename T >
  /* virtual */ void* property< T >::untyped_get_backend()
  {
    return &backend_;
  }

  template< typename T >
  void property< T >::set_from_backend( const T& value )
  {
    value_ = value;

    std::for_each(
      notify_change_to_frontend_.begin(),
      notify_change_to_frontend_.end(),
      [&]( notify_list_type::value_type& f )
      {
        f( value_ );
      }
    );
  }
}