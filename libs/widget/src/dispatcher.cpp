
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <next/widget/widget.hpp>
#include <next/widget/dispatcher.hpp>


namespace next
{
  namespace widgets
  {
    std::weak_ptr< thread_group > dispatcher::create_thread_group()
    {
      return static_cast< next::dispatcher* >( this )->create_thread_group();
    }

    void dispatcher::send_event_impl( widget& from, widget& to, std::unique_ptr< next::abstract_event_data > event_data )
    {
      next::dispatcher::send_event_impl( boost::optional< event_handler& >( from ), to, std::move( event_data ) );
    }

    // TODO REFACTOR THIS.
    // event_handler should only care about create_thread_group method
    // create just an interface for this one and use it instead of requiring a dispatcher in the event_handler constructor
    next::dispatcher& dispatcher::get_base_dispatcher()
    {
      return *this;
    }
  }
}