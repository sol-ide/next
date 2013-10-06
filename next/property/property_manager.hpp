
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <next/property/abstract_property.hpp>

#include <string>
#include <unordered_map>
#include <memory>

namespace next
{
  class properties_manager
  {
  public:
    template< typename Property >
    typename Property::value_type property() const;

    template< typename Property >
    void property( const typename Property::value_type& value );

    template< typename Property >
    bool has_property() const;

    template< typename Property >
    void listen( const std::function < void ( const typename Property::value_type& ) >& f );

    template< typename Property >
    property_backend< Property >& get_property_backend() const;

  private:
    typedef std::unordered_map< std::string, std::unique_ptr< abstract_property > > property_list_type;
    property_list_type properties_;
  };
}

#include <next/property/property_manager.hxx>