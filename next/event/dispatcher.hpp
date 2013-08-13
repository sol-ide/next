
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "abstract_event_data.hpp"
#include "event_data.hpp"
#include "message_handling_thread.hpp"
#include "thread_group.hpp"
#include "set_unique_ptr.hpp"
#include <boost/noncopyable.hpp>
#include <boost/fusion/container/set.hpp>
#include <boost/fusion/include/set.hpp>
//#include <boost/fusion/sequence/intrinsic/has_key.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/has_key.hpp>
#include <boost/phoenix/object/construct.hpp>
#include <memory>
#include <mutex>
#include <utility>
#include <unordered_set>

namespace next
{
	class dispatcher;
	class event_handler;

    namespace details
    {
        template< typename T >
        struct hash_weak
        {
            std::size_t operator( )( std::weak_ptr< T > w )
            {
                auto ptr = w.lock();
                return std::hash< decltype( ptr ) >()( ptr );
            }
        };

        template< typename T >
        struct equal_to_weak
        {
            bool operator( )( std::weak_ptr< T > w1, std::weak_ptr< T > w2 )
            {
                auto ptr1 = w1.lock();
                auto ptr2 = w2.lock();
                return ptr1.get() == ptr2.get();
            }
        };
    }

	template< typename Event >
	class send_event_t : boost::noncopyable
	{
	private:
		typedef std::unique_ptr< next::abstract_event_data > abstract_event_data_ptr;

		send_event_t();

	public:
		send_event_t( dispatcher& d, abstract_event_data_ptr event_data_ptr )
			: event_data_( std::move( event_data_ptr ) )
			, d_( d )
		{

		}

		send_event_t( send_event_t && other )
			: event_data_( std::move( other.event_data_ ) )
			, d_( std::move( other.d_ ) )
		{
		}

		void to( event_handler& h )
		{
			d_.send_event_impl( h, std::move( event_data_ ) );
		}

	private:
		abstract_event_data_ptr event_data_;
		dispatcher&				d_;
	};

	struct thread_pool_size_t
	{
		std::size_t pool_size;
	};

	class dispatcher : boost::noncopyable
	{
	public:
        ~dispatcher();

		template< typename... Args >
		dispatcher( Args&&... args )
		{
			typedef boost::fusion::set< Args... > Arguments;

#ifdef _MSC_VER
            // workaround msvc 2013 preview bug
            Arguments arguments( args... );
#else
            Arguments arguments( std::forward< Args >( args )... );
#endif

			std::size_t pool_size = 1;

            if( boost::fusion::result_of::has_key< Arguments, thread_pool_size_t >::value )
			{
                pool_size = boost::fusion::at_key< thread_pool_size_t >( arguments ).pool_size;
			}

            for( std::size_t index = 0; index < pool_size; ++index )
            {
                message_handling_thread* thread_ptr = nullptr;
                {
                    std::unique_lock< std::mutex > lock( threads_lock_ );
                    auto result = running_threads_.emplace( new message_handling_thread( *this ) );
                    thread_ptr = *result.first;
                }
                thread_ptr->wait_for_nothing_to_do();
            }

            
            
			// TODO: wait all threads instanciated
		}

		template< typename Event, typename... Args >
		send_event_t< Event > send_event( Args && ... args )
		{
			return send_event_t< Event >( *this, std::make_unique< next::event_data< Event > >( std::forward< Args >( args )... ) );
		}

        std::weak_ptr< thread_group > create_thread_group();

	private:

		template< typename Event >
		friend class send_event_t;

        void send_event_impl( event_handler& h, std::unique_ptr< next::abstract_event_data > event_data );


        friend class message_handling_thread;

        void register_in_waiting_task_poll( message_handling_thread* handler );
        void remove_from_waiting_task_poll( message_handling_thread* handler );


        friend thread_group;

        void move_waiting_to_dispatching_group_thread( thread_group* group );
        void remove_group_thread_from_currently_dispatching( thread_group* group );
        std::shared_ptr< thread_group > check_for_waiting_group();

	private:
        // typedef next::details::set_unique_ptr< message_handling_thread > message_handling_thread_ptr;
        // typedef std::unique_ptr< message_handling_thread > message_handling_thread_ptr;
        typedef message_handling_thread* message_handling_thread_ptr;

        std::unordered_set< message_handling_thread_ptr > waiting_threads_;
		std::unordered_set< message_handling_thread_ptr > running_threads_;
		mutable std::mutex                                threads_lock_;

        // typedef std::weak_ptr< thread_group > thread_group_ptr;
        // std::unordered_set < thread_group_ptr, details::hash_weak< thread_group >, details::equal_to_weak< thread_group > >  thread_groups_;
        typedef std::shared_ptr< thread_group > thread_group_ptr;
        std::unordered_set< thread_group_ptr >  thread_groups_;
        std::unordered_set< thread_group_ptr >  waiting_for_thread_thread_groups_;
        std::unordered_set< thread_group* >     waiting_for_dispatch_thread_groups_;
        std::unordered_set< thread_group* >     currently_dispatching_thread_groups_;
        mutable std::mutex                      thread_group_mutex_;
	};
}