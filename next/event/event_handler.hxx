
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
        template< typename EventHandler >
        /* virtual */ abstract_slot< EventHandler >::~abstract_slot()
        {

        }
    }

    template< typename EventHandler >
    event_handler_impl< EventHandler >::event_handler_impl( next::events::message_handling_client< EventHandler >& client )
        : group_( client.create_thread_group() )
    {
    }
    
    template< typename EventHandler >
    event_handler_impl< EventHandler >::~event_handler_impl()
    {
    }

    template< typename EventHandler >
    void event_handler_impl< EventHandler >::call( const std::string& event_name, void* from, void* to, void* untyped_parameters, void* untyped_promise )
    {
      auto iter_slot_owner = slots_.find( event_name );
      if( iter_slot_owner != std::end( slots_ ) )
      {
        iter_slot_owner->second->call(
          *static_cast< boost::optional< EventHandler >* >( from ),
          *static_cast< EventHandler* >( to ),
          untyped_parameters,
          untyped_promise
        );
      }
    }

    template< typename EventHandler >
    std::weak_ptr< thread_group< EventHandler > > event_handler_impl< EventHandler >::get_thread_group()
    {
        return group_;
    }

    /////////////////////////////////////////////////////////////////////////////////////

    template< typename EventHandler >
    event_handler_base< EventHandler >::event_handler_base( next::events::message_handling_client< EventHandler >& client )
       : handler_( std::make_shared < event_handler_impl< EventHandler > >( client ) )
    {

    }

    template< typename EventHandler >
    event_handler_base< EventHandler >::event_handler_base( event_handler_base< EventHandler >&& other )
      : handler_( std::move( other.handler_ ) )
    {

    }

    template< typename EventHandler >
    event_handler_base< EventHandler >::event_handler_base( const event_handler_base< EventHandler >& other )
        : handler_( other.handler_ )
    {
    }

    template< typename EventHandler >
    event_handler_base< EventHandler >::~event_handler_base()
    {
    }
    
    template< typename EventHandler >
    event_handler_base< EventHandler >& event_handler_base< EventHandler >::operator=( const event_handler_base< EventHandler >& other )
    {
      handler_ = other.handler_;
      return *this;
    }

    template< typename EventHandler >
    void event_handler_base< EventHandler >::call( const std::string& event_name, boost::optional< EventHandler >& from, EventHandler& to, void* untyped_parameters, void* untyped_promise )
    {
      handler_->call( event_name, &from, &to, untyped_parameters, untyped_promise );
    }

    template< typename EventHandler >
    std::weak_ptr< thread_group< EventHandler > > event_handler_base< EventHandler >::get_thread_group()
    {
        return handler_->get_thread_group();
    }
}
