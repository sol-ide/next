
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <next/property/abstract_property.hpp>

#include <boost/optional.hpp>
#include <boost/none_t.hpp>
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>

namespace next
{
  template< typename ConcreteManager = boost::none_t >
  class properties_manager
  {
  public:
    template< typename Property >
    boost::optional< typename Property::value_type > property() const;

    template< typename Property >
    void property( const typename Property::value_type& value );

    template< typename Property >
    bool has_property() const;

    template< typename Property >
    void listen( const std::function < void ( const typename Property::value_type& ) >& f );

    template< typename Property >
    boost::optional< property_backend< Property >& > get_property_backend() const;

  private:
    typedef std::unordered_map< std::string, std::unique_ptr< abstract_property > > property_list_type;
    property_list_type properties_;
    mutable std::mutex properties_mutex_;
  };
}

#include <next/property/property_manager.hxx>