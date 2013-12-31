
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#define BOOST_RESULT_OF_USE_DECLTYPE

#include <next/get_typename/get_typename.hpp>
#include <next/event/config.hpp>
#include <next/event/event.hpp>
#include <next/cpp/make_unique.hpp>
#include <next/event/message_handling_client.hpp>

#include <boost/fusion/functional/invocation/invoke.hpp>
#include <boost/optional.hpp>

#include <string>
#include <unordered_map>
#include <deque>
#include <memory>
#include <algorithm>
#include <future>

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable:4251)
#endif

namespace next
{
  namespace events
  {
    template< typename EventHandler >
    class message_handling_client;
  }

  template< typename EventHandler >
  class thread_group;

  template< typename EventHandler >
  class event_handler_base;

	namespace details
	{
    template< typename EventHandler >
    class  abstract_slot
		{
		public:
      virtual ~abstract_slot();

      virtual void call( boost::optional< EventHandler >& from, EventHandler& to, void* untyped_parameters, void* untyped_result ) = 0;
		};

    template< typename EventHandler >
    class abstract_slot_owner
    {
    public:
      virtual void call( boost::optional< EventHandler >& from, EventHandler& to, void* untyped_parameters, void* untyped_promise ) = 0;
      virtual void insert( std::unique_ptr< abstract_slot< EventHandler > > slot ) = 0;
    };

    template< typename EventHandler, typename ReturnType >
    struct call_agregate_result_helper
    {
      template< typename Aggregator >
      static void aggregate( Aggregator && aggregator, std::unique_ptr< abstract_slot< EventHandler > >& slot, boost::optional< EventHandler >& from, EventHandler& to, void* untyped_parameters )
      {
        ReturnType result;
        slot->call( from, to, untyped_parameters, &result );
        aggregator( result );
      }
    };

    template< typename EventHandler >
    struct call_agregate_result_helper< EventHandler, void >
    {
      template< typename Aggregator >
      static void aggregate( Aggregator && aggregator, std::unique_ptr< abstract_slot< EventHandler > >& slot, boost::optional< EventHandler >& from, EventHandler& to, void* untyped_parameters )
      {
        slot->call( from, to, untyped_parameters, nullptr );
        aggregator( true );
      }
    };

    template< typename EventHandler, typename Event, typename Aggregator >
    class slot_owner : public abstract_slot_owner< EventHandler >
    {
    public:
      virtual void call( boost::optional < EventHandler >& from, EventHandler& to, void* untyped_parameters, void* untyped_promise )
      {
        Aggregator aggregator;
        std::for_each(
          std::begin( slots_ ),
          std::end( slots_ ),
          [ &aggregator, untyped_parameters, &from, &to ]( std::unique_ptr< abstract_slot< EventHandler > >& slot )
          {
            call_agregate_result_helper< EventHandler, typename Event::return_type >::template aggregate( aggregator, slot, from, to, untyped_parameters );
          }
        );
        typedef typename Event::promise_type decorated_promise;
        auto promise = static_cast< decorated_promise* >( untyped_promise );
        promise->set_value( aggregator.value() );
      }

      virtual void insert( std::unique_ptr < abstract_slot< EventHandler > > slot )
      {
        slots_.push_back( std::move( slot ) );
      }

    private:
      std::deque< std::unique_ptr< abstract_slot< EventHandler > > > slots_;
    };

		template< typename EventHandler, typename Event, typename F >
		class slot : public abstract_slot< EventHandler >
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

      template< typename ParametersType >
      void select_call( ParametersType& parameters, void* result )
      {
        boost::fusion::invoke( f_, parameters );
      }

      virtual void call( boost::optional< EventHandler >& from, EventHandler& to, void* untyped_parameters, void* untyped_result )
			{
        typename Event::parameter_types* parameters = static_cast< typename Event::parameter_types* >( untyped_parameters );
        typename Event::return_type* result = static_cast< typename Event::return_type* >( untyped_result );
        select_call( *parameters, result );
			}

		private:
			F f_;
		};
	}

  template< typename EventHandler >
  class event_handler_impl
	{
	private:
    typedef std::unordered_map< std::string, std::shared_ptr< details::abstract_slot_owner< EventHandler > > > slots_map;

	public:
    event_handler_impl( next::events::message_handling_client< EventHandler >& client );
    event_handler_impl( event_handler_impl && other )
    {

    }

    ~event_handler_impl();

    template< typename Event >
    void listen( std::unique_ptr< details::abstract_slot< EventHandler > > slot );

    void call( const std::string& event_name, void* from, void* to, void* untyped_parameters, void* untyped_promise );

    std::weak_ptr< thread_group< EventHandler > > get_thread_group();

  private:
    event_handler_impl( const event_handler_impl& other );

	private:
    slots_map                                     slots_;
    std::weak_ptr< thread_group< EventHandler > > group_;
	};

  template< typename EventHandler >
  template< typename Event >
  void event_handler_impl< EventHandler >::listen( std::unique_ptr< details::abstract_slot< EventHandler > > slot )
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
        std::make_unique< details::slot_owner< EventHandler, Event, aggregator_type > >( )
      );

    }
    iter_slot->second->insert( std::move( slot ) );
  }

  template< typename EventHandler >
  class event_handler_base
  {
  public:
    event_handler_base( next::events::message_handling_client< EventHandler >& client );
    event_handler_base( const event_handler_base& other );
    event_handler_base( event_handler_base&& other );
        
    virtual ~event_handler_base();

    event_handler_base& operator=( const event_handler_base& other );

    template< typename Event, typename F >
    void listen( F&& f )
    {
      handler_->template listen< Event >( std::make_unique< details::slot< EventHandler, Event, F > >( std::forward< F >( f ) ) );
    }

    void call( const std::string& event_name, boost::optional< EventHandler >& from, EventHandler& to, void* untyped_parameters, void* untyped_promise );

    std::weak_ptr< thread_group< EventHandler > > get_thread_group();

  protected:
    std::shared_ptr< event_handler_impl< EventHandler > > handler_;
  };

  class event_handler : public event_handler_base< event_handler >
  {
  public:
    event_handler( next::events::message_handling_client< event_handler >& client )
      : event_handler_base{ client }
    {
    }
  };
}

extern template class next::event_handler_base< next::event_handler >;
extern template class next::event_handler_impl< next::event_handler >;
extern template class next::details::abstract_slot< next::event_handler >;

#ifdef _MSC_VER
# pragma warning(pop)
#endif
