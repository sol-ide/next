
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <next/property/abstract_property.hpp>
#include <next/get_typename/get_typename.hpp>

namespace next
{
  template< typename Property >
  typename Property::value_type properties_manager::property() const
  {
    const std::string& property_name = next::get_typename< Property >();
    auto iter = properties_.find( property_name );
    if( iter != properties_.end() )
    {
      return iter->second->get< typename Property::value_type >();
    }

    // should throw...
    return typename Property::value_type();
  }

  template< typename Property >
  void properties_manager::property( const typename Property::value_type& value )
  {
    const std::string& property_name = next::get_typename< Property >();
    property_list_type::iterator iter = properties_.find( property_name );
    if( iter == properties_.end() )
    {
      std::tie( iter, std::ignore ) = properties_.emplace( property_name, std::make_unique< next::property< typename Property::value_type > >( value ) );
    }
    iter->second->set< typename Property::value_type >( value );
  }

  template< typename Property >
  bool properties_manager::has_property() const
  {
    const std::string& property_name = next::get_typename< Property >();
    property_list_type::const_iterator iter = properties_.find( property_name );
    return iter != properties_.end();
  }

  template< typename Property >
  property_backend< Property >& properties_manager::get_property_backend() const
  {
    const std::string& property_name = next::get_typename< Property >( );
    auto iter = properties_.find( property_name );
    if( iter == properties_.end() )
    {
      // should throw...
        
    }
    return iter->second->get_backend< Property >( );
  }

  template< typename Property >
  void properties_manager::listen( const std::function < void ( const typename Property::value_type& ) >& f )
  {
    const std::string& property_name = next::get_typename< Property >( );
    auto iter = properties_.find( property_name );
    if( iter != properties_.end() )
    {
      // should throw...

    }
    return iter->second->listen< typename Property::value_type >( f );
  }
}