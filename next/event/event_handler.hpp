
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#define BOOST_RESULT_OF_USE_DECLTYPE

#include <next/event/config.hpp>
#include <next/event/event.hpp>

#include <boost/fusion/functional/invocation/invoke.hpp>

#include <string>
#include <unordered_map>
#include <memory>
#include <algorithm>


#pragma warning(push)
#pragma warning(disable:4251)

namespace next
{
    class dispatcher;
    class thread_group;

	namespace details
	{
        class NEXT_EVENT_EXPORT abstract_slot
		{
		public:
			virtual ~abstract_slot()
			{
			}



			virtual void call( void* untyped_parameters ) = 0;
		};

		template< typename Event, typename F >
		class slot : public abstract_slot
		{
		public:

			slot( F&& f )
				: f_( std::forward< F >( f ) )
			{

			}

			virtual void call( void* untyped_parameters )
			{
				typename Event::parameter_types* parameters = static_cast< typename Event::parameter_types* >( untyped_parameters );
				boost::fusion::invoke( f_, *parameters );
			}

		private:
			F f_;
		};
	}

    class NEXT_EVENT_EXPORT event_handler_impl
	{
	private:
		typedef std::unordered_multimap< std::string, std::unique_ptr< details::abstract_slot > > slots_type;

	public:
        event_handler_impl( next::dispatcher& d );

        event_handler_impl( const event_handler_impl& other );

        ~event_handler_impl();

		template< typename Event, typename F >
        void listen( F && f );

        void call( const std::string& event_name, void* untyped_parameters );

        std::weak_ptr< thread_group > get_thread_group();

	private:
		slots_type                    slots_;
        std::weak_ptr< thread_group > group_;
	};

    template< typename Event, typename F >
    void event_handler_impl::listen( F && f )
    {
        slots_.emplace(
            next::get_typename< Event >( ),
            std::make_unique< details::slot< Event, F > >( std::forward< F >( f ) )
        );
    }

    class NEXT_EVENT_EXPORT event_handler
    {
    public:
        event_handler( next::dispatcher& d );
        event_handler( const event_handler& other );

        ~event_handler();

        template< typename Event, typename F >
        void listen( F && f )
        {
            handler_->listen< Event >( std::forward< F >( f ) );
        }

        void call( const std::string& event_name, void* untyped_parameters );

        std::weak_ptr< thread_group > get_thread_group();

    private:
        std::shared_ptr< event_handler_impl > handler_;
    };
}

#pragma warning(pop)