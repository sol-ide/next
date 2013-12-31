//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <next/event/config.hpp>
#include <memory>


namespace next
{
  template< typename EventHandler >
  class message_handling_thread;

  template< typename EventHandler >
  class thread_group;

  namespace events
  {
    template< typename EventHandler >
    class message_handling_client
    {
    public:
      virtual std::weak_ptr< thread_group< EventHandler > > create_thread_group() = 0;

    private:
      template< typename T >
      friend
      class next::message_handling_thread;

      virtual void register_in_waiting_task_poll( message_handling_thread< EventHandler >* handler ) = 0;
      virtual void remove_from_waiting_task_poll( message_handling_thread< EventHandler >* handler ) = 0;


      template< typename T >
      friend
      class thread_group;

      virtual void move_waiting_to_dispatching_group_thread( thread_group< EventHandler >* group ) = 0;
      virtual void remove_group_thread_from_currently_dispatching( thread_group< EventHandler >* group ) = 0;
      virtual std::shared_ptr< thread_group< EventHandler > > check_for_waiting_group() = 0;
      virtual void wait_until_there_is_no_waiting_group() = 0;
    };
  }
}

