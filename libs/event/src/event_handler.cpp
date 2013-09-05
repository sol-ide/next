
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <next/event/event_handler.hpp>
#include <next/event/dispatcher.hpp>

namespace next
{
    namespace details
    {
        /* virtual */ abstract_slot::~abstract_slot()
        {

        }
    }

    event_handler_impl::event_handler_impl( next::dispatcher& d )
        : group_( d.create_thread_group() )
    {
    }

    /*
    event_handler_impl::event_handler_impl( const event_handler_impl& other )
        : group_( other.group_ )
    {
    }
    */

    event_handler_impl::~event_handler_impl()
    {
    }

    void event_handler_impl::call( const std::string& event_name, void* untyped_parameters, void* untyped_promise )
    {
      auto iter_slot_owner = slots_.find( event_name );
      if( iter_slot_owner != std::end( slots_ ) )
      {
        iter_slot_owner->second->call( untyped_parameters, untyped_promise );
      }
    }

    std::weak_ptr< thread_group > event_handler_impl::get_thread_group()
    {
        return group_;
    }

    /////////////////////////////////////////////////////////////////////////////////////

    event_handler::event_handler( next::dispatcher& d )
       : handler_( std::make_shared < event_handler_impl >( d ) )
    {

    }

    event_handler::event_handler( event_handler&& other )
      : handler_( std::move( other.handler_ ) )
    {

    }

    event_handler::event_handler( const event_handler& other )
        : handler_( other.handler_ )
    {
    }

    event_handler::~event_handler()
    {
    }

    void event_handler::call( const std::string& event_name, void* untyped_parameters, void* untyped_promise )
    {
      handler_->call( event_name, untyped_parameters, untyped_promise );
    }

    std::weak_ptr< thread_group > event_handler::get_thread_group()
    {
        return handler_->get_thread_group();
    }
}
