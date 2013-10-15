
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <next/property/property.hpp>
#include <next/property/property_manager.hpp>
#include <next/event/event_handler.hpp>
#include <next/event/dispatcher.hpp>

namespace next
{
  namespace widgets
  {
    template< typename F >
    struct property : public next::property< F >
    {
      typedef next::event < void( value_type ) > change;
    };

    class widget : public next::properties_manager< widget >, public next::event_handler
    {
    public:
      using properties_manager::property;
      using properties_manager::has_property;
      using properties_manager::listen;

      using event_handler::listen;

      widget( dispatcher& d )
        : next::event_handler{ d }
        , dispatcher_{ d }
      {
      }

    private:

      template< typename ConcreteManager >
      friend struct next::details::property_created;

      template< typename Property >
      void property_created( Property& prop )
      {
        prop.listen< typename Property::value_type >( [ &]( const typename Property::value_type& value ){
          dispatcher_.send_event< typename Property::change >( value ).to( *this );
        });
      }

    private:
      next::dispatcher& dispatcher_;
    };
  }
}