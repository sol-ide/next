
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <next/event/dispatcher.hpp>
#include <next/event/message_handling_thread.hpp>
#include <next/event/abstract_event_data.hpp>
#include <next/event/message_handling_client.hpp>

namespace next
{

  template< typename EventHandler >
	message_handling_thread< EventHandler >::message_handling_thread( events::message_handling_client< EventHandler >& client )
		: client_( client )
		, thread_as_been_asked_to_end_( false )
	{
    std::condition_variable wait_thread_launch;
    std::mutex thread_launch_mutex;

    {
      std::unique_lock< std::mutex > thread_launch_lock( thread_launch_mutex );
      thread_ = std::thread( &message_handling_thread< EventHandler >::thread_loop, this, std::ref( wait_thread_launch ), std::ref( thread_launch_mutex ) );
      wait_thread_launch.wait( thread_launch_lock );
    }
  }

  template< typename EventHandler >
  message_handling_thread< EventHandler >::~message_handling_thread()
  {
    ask_end_thread();
    thread_.join();
  }

  template< typename EventHandler >
	void message_handling_thread< EventHandler >::ask_end_thread()
	{
		{
			std::unique_lock< std::mutex > lock( thread_as_been_asked_to_end_mutex_ );
			thread_as_been_asked_to_end_ = true;
		}
		wake_up_thread();
	}

  template< typename EventHandler >
	bool message_handling_thread< EventHandler >::thread_as_not_been_asked_to_end() const
	{
		std::unique_lock< std::mutex > lock( thread_as_been_asked_to_end_mutex_ );
		return thread_as_been_asked_to_end_ == false;
	}

  template< typename EventHandler >
	void message_handling_thread< EventHandler >::wait_for_message( std::unique_lock< std::mutex >& lock )
	{
    if( thread_as_not_been_asked_to_end() == true )
    {
      client_.register_in_waiting_task_poll( this );
    }

    wait_for_message_condition_.wait( lock );

    // if we are stopping, we don't start the thread...
    if( thread_as_not_been_asked_to_end() == true )
    {
      client_.remove_from_waiting_task_poll( this );
    }
	}

  template< typename EventHandler >
  void message_handling_thread< EventHandler >::wait_for_nothing_to_do()
  {
    std::unique_lock< std::mutex > lock( wait_for_message_lock_ );
  }


  template< typename EventHandler >
  void message_handling_thread< EventHandler >::wake_up_thread( std::shared_ptr< thread_group< EventHandler > > group )
	{
		std::unique_lock< std::mutex > lock( wait_for_message_lock_ );
    currenlty_handled_group_ = group;
		wait_for_message_condition_.notify_one();
	}

  template< typename EventHandler >
  void message_handling_thread< EventHandler >::thread_loop( std::condition_variable& wait_thread_launch, std::mutex& thread_launch_mutex )
	{
    std::unique_lock< std::mutex > lock( wait_for_message_lock_ );

    {
      std::unique_lock< std::mutex > thread_launch_lock( thread_launch_mutex );
      wait_thread_launch.notify_one();
    }
     
    while( thread_as_not_been_asked_to_end() || currenlty_handled_group_ != nullptr )
		{
      if( currenlty_handled_group_ != nullptr )
      {
        // si on trouve dans la pile de message un message dont le group n'est pas en cours de traitement
        details::thread_group_event_data< EventHandler > message;
        bool has_message_in_queue = currenlty_handled_group_->get_first_free_message( message );
        if( has_message_in_queue )
        {
          client_.move_waiting_to_dispatching_group_thread( currenlty_handled_group_.get() );
          message.event_data->dispatch_message_to( message.from, *message.to );
          client_.remove_group_thread_from_currently_dispatching( currenlty_handled_group_.get() );
        }
        else
        {
          currenlty_handled_group_ = client_.check_for_waiting_group();
          if( currenlty_handled_group_ == nullptr )
          {
            // sinon on attend que le dispatcher nous reveille
            wait_for_message( lock );
          }
        }
      }
      else
      {
        currenlty_handled_group_ = client_.check_for_waiting_group();
        if( currenlty_handled_group_ == nullptr )
        {
          // sinon on attend que le dispatcher nous reveille
          wait_for_message( lock );
        }
      }
		}
	}
}
