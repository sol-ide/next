
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <next/event/dispatcher.hpp>
#include <boost/phoenix/core/argument.hpp>
#include <boost/phoenix/object/delete.hpp>


namespace next
{
  dispatcher::~dispatcher()
  {
    // TODO: wait until each event has been received
    {
      std::unique_lock< std::mutex > read_lock( begin_deleted_mutex_ );
      is_being_deleted_ = true;
    }

    wait_until_there_is_no_waiting_group();

    using boost::phoenix::placeholders::arg1;
    std::for_each( std::begin( waiting_threads_ ), std::end( waiting_threads_ ), boost::phoenix::delete_( arg1 ) );
    // std::for_each( std::begin( waiting_threads_ ), std::end( waiting_threads_ ), [ ]( message_handling_thread_ptr ptr ){ /* delete ptr; */ } );
    std::for_each( std::begin( running_threads_ ), std::end( running_threads_ ), boost::phoenix::delete_( arg1 ) );
    // std::for_each( std::begin( running_threads_ ), std::end( running_threads_ ), [ ]( message_handling_thread_ptr ptr ){ delete ptr; } );
  }

  std::weak_ptr< thread_group > dispatcher::create_thread_group()
  {
    using namespace std::placeholders;

    auto thread_group_ptr = std::make_shared< thread_group >( );
    std::unique_lock< std::mutex > lock( thread_group_mutex_ );
    thread_groups_.insert( thread_group_ptr );
    return thread_group_ptr;
  }

  void dispatcher::send_event_impl( event_handler& h, std::unique_ptr< next::abstract_event_data > event_data )
  {
    auto group_weak = h.get_thread_group();
    {
      std::unique_lock< std::mutex > lock( thread_group_mutex_ );
      {
        auto group_shared = group_weak.lock();
        group_shared->store_event_data( h, std::move( event_data ) );


        auto iter = currently_dispatching_thread_groups_.find( group_shared.get() );
        if( iter != currently_dispatching_thread_groups_.end() )
        {
          // an event is already dispatched in this thread group... do nothing
        }
        else
        {
          auto insert_result = waiting_for_dispatch_thread_groups_.insert( group_shared.get() );

          if( insert_result.second == true )
          {
            // no event is waiting or dispatching in this thread group... wake up a thread if possible.
            std::unique_lock< std::mutex > lock( threads_mutex_ );
            if( waiting_threads_.begin() != waiting_threads_.end() )
            {
              std::unique_lock< std::mutex > being_deleted_lock( begin_deleted_mutex_ );
              if( is_being_deleted_ == false )
              {
                being_deleted_lock.unlock();

                auto iter_waiting_thread = waiting_threads_.begin();
                auto iter_running_thread = running_threads_.insert( std::move( *iter_waiting_thread ) );
                waiting_threads_.erase( iter_waiting_thread );
                auto& message_handling_thread_ptr = *iter_running_thread.first;
                message_handling_thread_ptr->wake_up_thread( group_shared );
              }
            }
            else
            {
              waiting_for_thread_thread_groups_.insert( group_shared );
            }
          }
          else
          {
              // an event is already waiting for dispatch... do nothing
          }
        }
      }
    }
  }

  void dispatcher::register_in_waiting_task_poll( message_handling_thread* handler )
  {
    std::unique_lock< std::mutex > read_lock( threads_mutex_ );
    auto iter = running_threads_.find( handler );
    if( iter != running_threads_.end() )
    {
      waiting_threads_.insert( std::move( *iter ) );
      running_threads_.erase( iter );
    }
    else
    {
#ifdef _WIN32
      __debugbreak();
#endif
    }
  }

  void dispatcher::remove_from_waiting_task_poll( message_handling_thread* handler )
  {
    std::unique_lock< std::mutex > lock( threads_mutex_ );
    auto iter = waiting_threads_.find( handler );
    if( iter != waiting_threads_.end() )
    {
      running_threads_.insert( std::move( *iter ) );
      waiting_threads_.erase( iter );
    }
  }

  void dispatcher::move_waiting_to_dispatching_group_thread( thread_group* group )
  {
    std::unique_lock< std::mutex > lock( thread_group_mutex_ );
    waiting_for_dispatch_thread_groups_.erase( group );
    currently_dispatching_thread_groups_.insert( group );
  }

  void dispatcher::remove_group_thread_from_currently_dispatching( thread_group* group )
  {
    std::unique_lock< std::mutex > lock( thread_group_mutex_ );
    if( group->has_pending_messages() == false )
    {
      currently_dispatching_thread_groups_.erase( group );

      // notify when there is no more thred group in the pipe...
      if( waiting_for_thread_thread_groups_.empty() && waiting_for_dispatch_thread_groups_.empty() && currently_dispatching_thread_groups_.empty() )
      {
        no_more_waiting_thread_group_condition_.notify_all();
      }
    }
  }

  std::shared_ptr< thread_group > dispatcher::check_for_waiting_group()
  {
    std::unique_lock< std::mutex > lock( thread_group_mutex_ );
    auto found_group = std::shared_ptr< thread_group >( );
    if( waiting_for_thread_thread_groups_.begin() != waiting_for_thread_thread_groups_.end() )
    {
      auto iter = waiting_for_thread_thread_groups_.begin();
      found_group = *iter;
      waiting_for_thread_thread_groups_.erase( iter );
      waiting_for_dispatch_thread_groups_.insert( found_group.get() );
    }
    return found_group;
  }

  void dispatcher::wait_until_there_is_no_waiting_group()
  {
    std::unique_lock< std::mutex > lock( thread_group_mutex_ );
    while( waiting_for_thread_thread_groups_.empty() == false && waiting_for_dispatch_thread_groups_.empty() == false && currently_dispatching_thread_groups_.empty() == false )
    {
      no_more_waiting_thread_group_condition_.wait( lock );
    }
  }
}
