
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <next/property/abstract_property.hpp>
#include <next/get_typename/get_typename.hpp>
#include <next/cpp/make_unique.hpp>

namespace next
{
  template< typename ConcreteManager >
  template< typename Property >
  boost::optional< typename Property::value_type > properties_manager_impl< ConcreteManager >::property() const
  {
    const std::string& property_name = next::get_typename< Property >();
    boost::optional< typename Property::value_type > value;
    std::unique_lock< std::mutex > lock( properties_mutex_ );
    auto iter = properties_.find( property_name );
    if( iter != properties_.end() )
    {
      value = iter->second->template get< typename Property::value_type >();
    }

    return value;
  }

  namespace details
  {
    template< typename ConcreteManager >
    struct property_created
    {
      template< typename Property >
      static void call( ConcreteManager& self, abstract_property& abstract_p )
      {
        ConcreteManager& concrete_self = static_cast < ConcreteManager& >( self );
        Property& p = static_cast< Property& >( abstract_p );
        concrete_self.property_created( p );
      }
    };

    template<>
    struct property_created< boost::none_t >
    {
      template< typename Property >
      static void call( boost::none_t& self, abstract_property& abstract_p )
      {
      }
    };
  }

  template< typename ConcreteManager >
  template< typename Property, typename Manager >
  void properties_manager_impl< ConcreteManager >::property( const typename Property::value_type& value, Manager&& self )
  {
    static_assert( std::is_same< typename std::remove_reference< Manager >::type, ConcreteManager >::value, "internal error: ConcreteManager and Mnager should be the same type" );
    const std::string& property_name = next::get_typename< Property >();
    std::unique_lock< std::mutex > lock( properties_mutex_ );
    property_list_type::iterator iter = properties_.find( property_name );
    if( iter == properties_.end() )
    {
      std::tie( iter, std::ignore ) = properties_.emplace( property_name, std::make_unique< next::property< typename Property::value_type > >( value ) );
      details::property_created< ConcreteManager >::template call< Property >( self, *( iter->second ) );
    }
    iter->second->template set< typename Property::value_type >( value );
  }

  template< typename ConcreteManager >
  template< typename Property >
  bool properties_manager_impl< ConcreteManager >::has_property() const
  {
    const std::string& property_name = next::get_typename< Property >();
    std::unique_lock< std::mutex > lock( properties_mutex_ );
    property_list_type::const_iterator iter = properties_.find( property_name );
    return iter != properties_.end();
  }

  template< typename ConcreteManager >
  template< typename Property >
  boost::optional< property_backend< Property >& > properties_manager_impl< ConcreteManager >::get_property_backend() const
  {
    const std::string& property_name = next::get_typename< Property >( );
    boost::optional< property_backend< Property >& > backend;
    std::unique_lock< std::mutex > lock( properties_mutex_ );
    auto iter = properties_.find( property_name );
    if( iter != properties_.end() )
    {
      backend = iter->second->template get_backend< Property >( );
        
    }
    return backend;
  }

  template< typename ConcreteManager >
  template< typename Property >
  void properties_manager_impl< ConcreteManager >::listen( const std::function < void ( const typename Property::value_type& ) >& f )
  {
    const std::string& property_name = next::get_typename< Property >( );
    std::unique_lock< std::mutex > lock( properties_mutex_ );
    auto iter = properties_.find( property_name );
    if( iter != properties_.end() )
    {
      return iter->second->template listen< typename Property::value_type >( f );
    }
    // should throw...
  }
}
