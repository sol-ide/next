
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <next/event/abstract_event_data.hpp>
#include <next/get_typename/get_typename.hpp>
#include <future>

namespace next
{

  template< typename EventHandler, typename Event >
  class event_data : public abstract_event_data< EventHandler >
  {
  public:
    typedef typename Event::parameter_types parameters_type;
    typedef typename Event::return_type     return_type;
    typedef typename Event::future_type     future_type;
    typedef typename Event::promise_type    promise_type;

    template< typename... Args >
    event_data( Args && ... args )
      : parameters_( std::forward< Args >( args )... )
      , result_( promise_.get_future() )
    {
    }

    virtual void dispatch_message_to( boost::optional< EventHandler >& from, EventHandler& to )
    {
      to.call( next::get_typename< Event >(), from, to, &parameters_, &promise_ );
    }

    virtual void get_future_result( void* value )
    {
      auto typed_value = static_cast< future_type* >( value );
      *typed_value = std::move( result_ );
      
    }

  private:

    parameters_type parameters_;
    promise_type    promise_;
    future_type     result_;
  };
}
