
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <next/event/config.hpp>
#include <next/event/abstract_event_data.hpp>
#include <next/event/event_data.hpp>
#include <next/event/message_handling_client.hpp>
#include <next/event/message_handling_thread.hpp>
#include <next/event/thread_group.hpp>
#include <boost/noncopyable.hpp>
#include <boost/fusion/container/set.hpp>
#include <boost/fusion/include/set.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/has_key.hpp>
#include <boost/phoenix/object/construct.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <memory>
#include <mutex>
#include <utility>
#include <unordered_set>
#include <future>

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable:4251)
# pragma warning(disable:4275)
#endif

namespace next
{
  template< typename EventHandler >
  class dispatcher;

    namespace details
    {
        template< typename T >
        struct hash_weak
        {
            std::size_t operator( )( std::weak_ptr< T >& w )
            {
                auto ptr = w.lock();
                return std::hash< decltype( ptr ) >()( ptr );
            }
        };

        template< typename T >
        struct equal_to_weak
        {
            bool operator( )( std::weak_ptr< T >& w1, std::weak_ptr< T >& w2 )
            {
                auto ptr1 = w1.lock();
                auto ptr2 = w2.lock();
                return ptr1.get() == ptr2.get();
            }
        };
    }

    template< typename EventHandler, typename Event >
    class send_event_from_t : boost::noncopyable
    {
    private:
      typedef std::unique_ptr< next::abstract_event_data< EventHandler > > abstract_event_data_ptr;

      send_event_from_t();

    public:
      send_event_from_t( dispatcher< EventHandler >& d, EventHandler& from, abstract_event_data_ptr event_data_ptr )
        : event_data_( std::move( event_data_ptr ) )
        , d_( d )
        , from_( from )
      {
      }

      send_event_from_t( send_event_from_t && other )
        : event_data_( std::move( other.event_data_ ) )
        , d_( other.d_ )
        , from_( other.from_ )
      {
      }

      typename Event::future_type to( EventHandler& h );

    private:
      abstract_event_data_ptr     event_data_;
      dispatcher< EventHandler >& d_;
      EventHandler&               from_;
    };

  template< typename EventHandler, typename Event >
  class send_event_t : boost::noncopyable
  {
  private:
    typedef std::unique_ptr< next::abstract_event_data< EventHandler > > abstract_event_data_ptr;

    send_event_t();

  public:
    send_event_t( dispatcher< EventHandler >& d, abstract_event_data_ptr event_data_ptr )
      : event_data_( std::move( event_data_ptr ) )
      , d_( d )
    {

    }

    send_event_t( send_event_t && other )
      : event_data_( std::move( other.event_data_ ) )
      , d_( other.d_ )
    {
    }

    send_event_from_t< EventHandler, Event > from( EventHandler& h )
    {
      return send_event_from_t< EventHandler, Event >( d_, h, std::move( event_data_ ) );
    }

    typename Event::future_type to( EventHandler& h );

  private:
    abstract_event_data_ptr event_data_;
    dispatcher< EventHandler >& d_;
  };

  struct thread_pool_size_t
  {
    typedef std::size_t type;
    type value;
  };

  namespace details
  {
    template < typename Parameter, bool ParameterIsInArguments, typename... Arguments >
    struct initialize_parameter_from_argument_impl;

    template < typename Parameter, typename... Arguments >
    struct initialize_parameter_from_argument_impl< Parameter, true, Arguments... >
    {
      static void initialize( typename Parameter::type& value, Arguments&... arguments )
      {
        value = boost::fusion::at_key< Parameter >(
#ifdef _MSC_VER
          // workaround msvc 2013 preview bug
          boost::fusion::set< Arguments... >( arguments... )
#else
          boost::fusion::set< Arguments... >( std::forward< Arguments >( arguments )... )
#endif
        ).value;
      }
      
      static typename Parameter::type initialize( Arguments&... arguments )
      {
        return boost::fusion::at_key< Parameter >(
#ifdef _MSC_VER
          // workaround msvc 2013 preview bug
          boost::fusion::set< Arguments... >( arguments... )
#else
          boost::fusion::set< Arguments... >( std::forward< Arguments >( arguments )... )
#endif
        ).value;
      }
    };

    template < typename Parameter, typename... Arguments >
    struct initialize_parameter_from_argument_impl< Parameter, false, Arguments... >
    {
      static void initialize( typename Parameter::type& /* value */, Arguments&... /* arguments */ )
      {
      }
    };

    template< typename Parameter, typename... Arguments >
    struct initialize_parameter_from_argument
      : initialize_parameter_from_argument_impl<
          Parameter,
          boost::fusion::result_of::has_key<
            boost::fusion::set< Arguments... >,
            Parameter
          >::value,
          Arguments...
      >
    {
    };
  }

  template< typename EventHandler >
  class dispatcher : boost::noncopyable, public next::events::message_handling_client< EventHandler >
  {
  public:
    virtual ~dispatcher();

    template< typename... Args >
    dispatcher( Args&&... args )
      : is_being_deleted_( false )
    {
      std::size_t pool_size = 1;
      details::initialize_parameter_from_argument< thread_pool_size_t, Args... >::initialize( pool_size, args... );

      for( std::size_t index = 0; index < pool_size; ++index )
      {
        message_handling_thread< EventHandler >* thread_ptr = nullptr;
        {
          std::unique_lock< std::mutex > lock( threads_mutex_ );
          auto result = running_threads_.emplace( new message_handling_thread< EventHandler >( *this ) );
          thread_ptr = *result.first;
        }
        thread_ptr->wait_for_nothing_to_do();
      }
      // TODO: wait all threads instanciated
    }

    template< typename Event, typename... Args >
    send_event_t< EventHandler, Event > send_event( Args && ... args )
    {
      return send_event_t< EventHandler, Event >( *this, std::make_unique< next::event_data< EventHandler, Event > >( std::forward< Args >( args )... ) );
    }

    std::weak_ptr< thread_group< EventHandler > > create_thread_group() override;

  protected:
    void send_event_impl( boost::optional< EventHandler& > from, EventHandler& to, std::unique_ptr< next::abstract_event_data< EventHandler > > event_data );

  private:
    template< typename Handler, typename Event >
    friend class send_event_t;

    template< typename Handler, typename Event >
    friend class send_event_from_t;

    template< typename T >
    friend class message_handling_thread;

    void register_in_waiting_task_poll( message_handling_thread< EventHandler >* handler ) override;
    void remove_from_waiting_task_poll( message_handling_thread< EventHandler >* handler ) override;

    template< typename T >
    friend
    class thread_group;

    void move_waiting_to_dispatching_group_thread( thread_group< EventHandler >* group ) override;
    void remove_group_thread_from_currently_dispatching( thread_group< EventHandler >* group ) override;
    std::shared_ptr< thread_group< EventHandler > > check_for_waiting_group() override;
    void wait_until_there_is_no_waiting_group() override;

  private:
    typedef message_handling_thread< EventHandler >* message_handling_thread_ptr;

    std::unordered_set< message_handling_thread_ptr > waiting_threads_;
    std::unordered_set< message_handling_thread_ptr > running_threads_;

    // sometimes we need to read 
    mutable std::mutex                                threads_mutex_;

    typedef std::shared_ptr< thread_group< EventHandler > > thread_group_ptr;
    std::unordered_set< thread_group_ptr >  thread_groups_;
    std::unordered_set< thread_group_ptr >  waiting_for_thread_thread_groups_;
    std::unordered_set< thread_group< EventHandler >* >     waiting_for_dispatch_thread_groups_;
    std::unordered_set< thread_group< EventHandler >* >     currently_dispatching_thread_groups_;
    mutable std::mutex                      thread_group_mutex_;
    std::condition_variable                 no_more_waiting_thread_group_condition_;


    bool                                    is_being_deleted_;
    mutable std::mutex                      begin_deleted_mutex_;
  };
  
  template< typename EventHandler, typename Event >
  typename Event::future_type send_event_from_t< EventHandler, Event >::to( EventHandler& h )
  {
    typename Event::future_type future;
    event_data_->get_future_result( &future );
    d_.send_event_impl( from_, h, std::move( event_data_ ) );
    return std::move( future );
  }

  template< typename EventHandler, typename Event >
  typename Event::future_type send_event_t< EventHandler, Event >::to( EventHandler& h )
  {
    typename Event::future_type future;
    event_data_->get_future_result( &future );
    d_.send_event_impl( boost::none, h, std::move( event_data_ ) );
    return std::move( future );
  }
}

#ifdef _MSC_VER
# pragma warning(pop)
#endif

#include <next/event/dispatcher.hxx>
