
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "dispatcher.hpp"
#include <boost/phoenix/core/argument.hpp>
#include <boost/phoenix/object/delete.hpp>


namespace next
{
    dispatcher::~dispatcher()
    {
        // TODO: wait until each event has been received

        using boost::phoenix::placeholders::arg1;
        std::for_each( std::begin( waiting_threads_ ), std::end( waiting_threads_ ), boost::phoenix::delete_( arg1 ) );
        // std::for_each( std::begin( waiting_threads_ ), std::end( waiting_threads_ ), [ ]( message_handling_thread_ptr ptr ){ /* delete ptr; */ } );
        std::for_each( std::begin( running_threads_ ), std::end( running_threads_ ), boost::phoenix::delete_( arg1 ) );
        // std::for_each( std::begin( running_threads_ ), std::end( running_threads_ ), [ ]( message_handling_thread_ptr ptr ){ delete ptr; } );
    }

    std::weak_ptr< thread_group > dispatcher::create_thread_group()
    {
        using namespace std::placeholders;

        // auto thread_group_ptr = std::shared_ptr< thread_group >( new thread_group(), std::bind( &dispatcher::remove_thread_group_and_delete, this, _1 ) );
        auto thread_group_ptr = std::make_shared< thread_group >( );
        std::unique_lock< std::mutex > lock( thread_group_mutex_ );
        thread_groups_.insert( thread_group_ptr );
        return thread_group_ptr;
    }

    void dispatcher::send_event_impl( event_handler& h, std::unique_ptr< next::abstract_event_data > event_data )
    {
        // event_data->dispatch_message_to( h );

        auto& group = h.get_thread_group();

        {
            std::unique_lock< std::mutex > lock( thread_group_mutex_ );
            {
                auto group_lock = group.lock();
                group_lock->store_event_data( h, std::move( event_data ) );


                auto iter = currently_dispatching_thread_groups_.find( group_lock.get() );
                if( iter != currently_dispatching_thread_groups_.end() )
                {
                    // an event is already dispatched in this thread group... do nothing
                }
                else
                {
                    auto insert_result = waiting_for_dispatch_thread_groups_.insert( group_lock.get() );

                    if( insert_result.second == true )
                    {
                        // no event is waiting or dispatching in this thread group... wake up a thread if possible.
                        std::unique_lock< std::mutex > lock( threads_lock_ );
                        if( waiting_threads_.begin() != waiting_threads_.end() )
                        {

                            auto iter_waiting_thread = waiting_threads_.begin();
                            auto iter_running_thread = running_threads_.insert( std::move( *iter_waiting_thread ) );
                            waiting_threads_.erase( iter_waiting_thread );
                            auto& message_handling_thread_ptr = *iter_running_thread.first;
                            message_handling_thread_ptr->wake_up_thread( group_lock );
                        }
                        else
                        {
                            waiting_for_thread_thread_groups_.insert( group_lock );
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
        std::unique_lock< std::mutex > lock( threads_lock_ );
        auto iter = running_threads_.find( handler );
        if( iter != running_threads_.end() )
        {
            waiting_threads_.insert( std::move( *iter ) );
            running_threads_.erase( iter );
        }
        else
        {
            _asm int 3;
        }
    }

    void dispatcher::remove_from_waiting_task_poll( message_handling_thread* handler )
    {
        std::unique_lock< std::mutex > lock( threads_lock_ );
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
        currently_dispatching_thread_groups_.erase( group );
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
}