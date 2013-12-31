
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <next/event/config.hpp>
#include <next/event/thread_group.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace next
{
  namespace events
  {
    template< typename EventHandler >
	  class message_handling_client;
  }

  template< typename EventHandler >
  class message_handling_thread
  {
  public:
    message_handling_thread( events::message_handling_client< EventHandler >& client );
    ~message_handling_thread();

    void thread_loop( std::condition_variable& wait_thread_launch, std::mutex& thread_launch_mutex );
    void wake_up_thread( std::shared_ptr< thread_group< EventHandler > > group = nullptr  );
    void wait_for_nothing_to_do();
  private:
    void ask_end_thread();
    bool thread_as_not_been_asked_to_end() const;

    void wait_for_message( std::unique_lock< std::mutex >& lock );

  private:
    std::thread thread_;

    events::message_handling_client< EventHandler >& client_;

    bool thread_as_been_asked_to_end_;
    mutable std::mutex thread_as_been_asked_to_end_mutex_;


    std::condition_variable wait_for_message_condition_;
    mutable std::mutex wait_for_message_lock_;

    std::shared_ptr< thread_group< EventHandler > > currenlty_handled_group_;
  };
}

extern template class next::message_handling_thread< next::event_handler >;
