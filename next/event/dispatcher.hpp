
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <next/event/config.hpp>
#include <next/event/abstract_event_data.hpp>
#include <next/event/event_data.hpp>
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
  class dispatcher;
  class event_handler;

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

    template< typename Event >
    class send_event_from_t : boost::noncopyable
    {
    private:
      typedef std::unique_ptr< next::abstract_event_data > abstract_event_data_ptr;

      send_event_from_t();

    public:
      send_event_from_t( dispatcher& d, event_handler& from, abstract_event_data_ptr event_data_ptr )
        : event_data_( std::move( event_data_ptr ) )
        , d_( d )
        , from_( from )
      {
      }

      send_event_from_t( send_event_from_t && other )
        : event_data_( std::move( other.event_data_ ) )
        , d_( std::move( other.d_ ) )
        , from_( std::move( other.from_ ) )
      {
      }

      typename Event::future_type to( event_handler& h )
      {
        typename Event::future_type future;
        event_data_->get_future_result( &future );
        d_.send_event_impl( from_, h, std::move( event_data_ ) );
        return std::move( future );
      }

    private:
      abstract_event_data_ptr event_data_;
      event_handler&          from_;
      dispatcher&             d_;
    };

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
      , d_( other.d_ )
    {
    }

    send_event_from_t< Event > from( event_handler& h )
    {
      return send_event_from_t< Event >( d_, h, std::move( event_data_ ) );
    }

    typename Event::future_type to( event_handler& h )
    {
      typename Event::future_type future;
      event_data_->get_future_result( &future );
      d_.send_event_impl( boost::none, h, std::move( event_data_ ) );
      return std::move( future );
    }

  private:
    abstract_event_data_ptr event_data_;
    dispatcher&        d_;
  };

  struct thread_pool_size_t
  {
    typedef std::size_t type;
    type value;
  };

  template < typename Arguments, typename Parameter, bool ParameterIsInArguments >
  struct initialize_thread_pool_size_from_argument_impl;

  template < typename Arguments, typename Parameter >
  struct initialize_thread_pool_size_from_argument_impl< Arguments, Parameter, true >
  {
  };

  template < typename Arguments, typename Parameter >
  struct initialize_thread_pool_size_from_argument_impl< Arguments, Parameter, false >
  {
    static void initialize( typename Parameter::type& /* value */ )
    {
    }
  };

  template < typename Arguments, typename Parameter >
  struct initialize_thread_pool_size_from_argument : initialize_thread_pool_size_from_argument_impl < Arguments, Parameter, boost::fusion::result_of::has_key< Arguments, Parameter >::value >
  {
    static void initialize( typename Parameter::type& value )
    {
      value = boost::fusion::at_key< thread_pool_size_t >( arguments ).value;
    }
  };

  class NEXT_EVENT_EXPORT dispatcher : boost::noncopyable
  {
  public:
        ~dispatcher();

    template< typename... Args >
    dispatcher( Args&&... args )
      : is_being_deleted_( false )
    {
      typedef boost::fusion::set< Args... > Arguments;

#ifdef _MSC_VER
      // workaround msvc 2013 preview bug
      Arguments arguments( args... );
#else
      Arguments arguments( std::forward< Args >( args )... );
#endif

      std::size_t pool_size = 1;
      initialize_thread_pool_size_from_argument< Arguments, thread_pool_size_t >::initialize( pool_size );

      for( std::size_t index = 0; index < pool_size; ++index )
      {
        message_handling_thread* thread_ptr = nullptr;
        {
          std::unique_lock< std::mutex > lock( threads_mutex_ );
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

  protected:
    void send_event_impl( boost::optional< event_handler& > from, event_handler& to, std::unique_ptr< next::abstract_event_data > event_data );

  private:
    template< typename Event >
    friend class send_event_t;

    template< typename Event >
    friend class send_event_from_t;


    friend class message_handling_thread;

    void register_in_waiting_task_poll( message_handling_thread* handler );
    void remove_from_waiting_task_poll( message_handling_thread* handler );


    friend thread_group;

    void move_waiting_to_dispatching_group_thread( thread_group* group );
    void remove_group_thread_from_currently_dispatching( thread_group* group );
    std::shared_ptr< thread_group > check_for_waiting_group();
    void wait_until_there_is_no_waiting_group();

  private:
    // typedef next::details::set_unique_ptr< message_handling_thread > message_handling_thread_ptr;
    // typedef std::unique_ptr< message_handling_thread > message_handling_thread_ptr;
    typedef message_handling_thread* message_handling_thread_ptr;

    std::unordered_set< message_handling_thread_ptr > waiting_threads_;
    std::unordered_set< message_handling_thread_ptr > running_threads_;

    // sometimes we need to read 
    mutable std::mutex                                threads_mutex_;

    // typedef std::weak_ptr< thread_group > thread_group_ptr;
    // std::unordered_set < thread_group_ptr, details::hash_weak< thread_group >, details::equal_to_weak< thread_group > >  thread_groups_;
    typedef std::shared_ptr< thread_group > thread_group_ptr;
    std::unordered_set< thread_group_ptr >  thread_groups_;
    std::unordered_set< thread_group_ptr >  waiting_for_thread_thread_groups_;
    std::unordered_set< thread_group* >     waiting_for_dispatch_thread_groups_;
    std::unordered_set< thread_group* >     currently_dispatching_thread_groups_;
    mutable std::mutex                      thread_group_mutex_;
    std::condition_variable                 no_more_waiting_thread_group_condition_;


    bool                                    is_being_deleted_;
    mutable std::mutex                      begin_deleted_mutex_;
  };

  template< typename Event >
  typename Event::future_type send_event_t< Event >::to( event_handler& h )
  {
    typename Event::future_type future;
    event_data_->get_future_result( &future );
    d_.send_event_impl( h, std::move( event_data_ ) );
    return std::move( future );
  }
}

#ifdef _MSC_VER
# pragma warning(pop)
#endif
