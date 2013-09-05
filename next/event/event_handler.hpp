
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#define BOOST_RESULT_OF_USE_DECLTYPE

#include <next/event/config.hpp>
#include <next/event/event.hpp>

#include <boost/fusion/functional/invocation/invoke.hpp>
#include <boost/optional.hpp>

#include <string>
#include <unordered_map>
#include <deque>
#include <memory>
#include <algorithm>
#include <future>


#pragma warning(push)
#pragma warning(disable:4251)

namespace next
{
    class dispatcher;
    class thread_group;

	namespace details
	{
    class abstract_slot;

    class NEXT_EVENT_EXPORT abstract_slot_owner
    {
    public:
      virtual void call( void* untyped_parameters, void* untyped_promise ) = 0;
      virtual void insert( std::unique_ptr< abstract_slot > slot ) = 0;
    };

    template< typename ReturnType >
    struct call_agregate_result_helper
    {
      template< typename Aggregator >
      static void aggregate( Aggregator && aggregator, std::unique_ptr< abstract_slot >& slot, void* untyped_parameters )
      {
        ReturnType result;
        slot->call( untyped_parameters, &result );
        aggregator( result );
      }
    };

    template<>
    struct call_agregate_result_helper< void >
    {
      template< typename Aggregator >
      static void aggregate( Aggregator && aggregator, std::unique_ptr< abstract_slot >& slot, void* untyped_parameters )
      {
        slot->call( untyped_parameters, nullptr );
        aggregator( true );
      }
    };

    template< typename Event, typename Aggregator >
    class slot_owner : public abstract_slot_owner
    {
    public:
      virtual void call( void* untyped_parameters, void* untyped_promise )
      {
        Aggregator aggregator;
        std::for_each(
          std::begin( slots_ ),
          std::end( slots_ ),
          [ &aggregator, untyped_parameters ]( std::unique_ptr< abstract_slot >& slot )
          {
            call_agregate_result_helper< typename Event::return_type >::template aggregate( aggregator, slot, untyped_parameters );
          }
        );
        typedef typename Event::promise_type decorated_promise;
        auto promise = static_cast< decorated_promise* >( untyped_promise );
        promise->set_value( aggregator.value() );
      }

      virtual void insert( std::unique_ptr < abstract_slot > slot )
      {
        slots_.push_back( std::move( slot ) );
      }

    private:
      std::deque< std::unique_ptr< abstract_slot > > slots_;
    };

    class NEXT_EVENT_EXPORT abstract_slot
		{
		public:
      virtual ~abstract_slot();

      virtual void call( void* untyped_parameters, void* untyped_result ) = 0;
		};

		template< typename Event, typename F >
		class slot : public abstract_slot
		{
		public:

      slot( F&& f )
				: f_( std::forward< F >( f ) )
			{

			}


      template< typename ParametersType, typename ReturnType >
      void select_call( ParametersType& parameters, ReturnType* result )
      {
        *result = boost::fusion::invoke( f_, parameters );
      }

      template< typename typename ParametersType >
      void select_call( ParametersType& parameters, void* result )
      {
        boost::fusion::invoke( f_, parameters );
      }

			virtual void call( void* untyped_parameters, void* untyped_result )
			{
        typename Event::parameter_types* parameters = static_cast< typename Event::parameter_types* >( untyped_parameters );
        typename Event::return_type* result = static_cast< typename Event::return_type* >( untyped_result );
        select_call( *parameters, result );
			}

		private:
			F f_;
		};
	}

  class NEXT_EVENT_EXPORT event_handler_impl
	{
	private:
    typedef std::unordered_map< std::string, std::shared_ptr< details::abstract_slot_owner > > slots_map;

	public:
    event_handler_impl( next::dispatcher& d );
    event_handler_impl( event_handler_impl && other )
    {

    }

    ~event_handler_impl();

    template< typename Event, typename F >
    void listen( F&& f );

    void call( const std::string& event_name, void* untyped_parameters, void* untyped_promise );

    std::weak_ptr< thread_group > get_thread_group();

  private:
    event_handler_impl( const event_handler_impl& other );

	private:
    slots_map                     slots_;
    std::weak_ptr< thread_group > group_;
	};


    template< typename Event, typename F >
    void event_handler_impl::listen( F && f )
    {
      typedef typename Event::return_type return_type;
      typedef typename Event::aggregator_type aggregator_type;

      const std::string& event_name = next::get_typename< Event >( );
      auto iter_slot = slots_.find( event_name );
      if( iter_slot == slots_.end() )
      {
        bool has_been_inserted = false;
        std::tie( iter_slot, has_been_inserted ) = slots_.emplace(
          event_name,
          std::make_unique< details::slot_owner< Event, aggregator_type > >( )
        );

      }
      iter_slot->second->insert( std::make_unique< details::slot< Event, F > >( std::forward< F >( f ) ) );
    }

    class NEXT_EVENT_EXPORT event_handler
    {
    public:
        event_handler( next::dispatcher& d );
        event_handler( const event_handler& other );
        event_handler( event_handler&& other );
        
        ~event_handler();

        template< typename Event, typename F >
        void listen( F&& f )
        {
          handler_->template listen< Event, F >( std::forward< F >( f ) );
        }

        void call( const std::string& event_name, void* untyped_parameters, void* untyped_promise );

        std::weak_ptr< thread_group > get_thread_group();

    private:
        std::shared_ptr< event_handler_impl > handler_;
    };
}

#pragma warning(pop)