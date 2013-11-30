
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <next/event/dispatcher.hpp>

namespace next
{
  namespace widgets
  {
    class widget;
    class dispatcher;

    namespace details
    {
      template< typename Event >
      class send_event_from_t : boost::noncopyable
      {
      private:
        typedef std::unique_ptr< next::abstract_event_data > abstract_event_data_ptr;

      public:
        send_event_from_t( next::widgets::dispatcher& d, widget& from, abstract_event_data_ptr event_data_ptr )
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

        typename Event::future_type to( widget& h )
        {
          typename Event::future_type future;
          event_data_->get_future_result( &future );
          d_.send_event_impl( from_, h, std::move( event_data_ ) );
          return std::move( future );
        }

      private:
        abstract_event_data_ptr     event_data_;
        widget&                     from_;
        next::widgets::dispatcher&  d_;
      };

      template< typename Event >
      class send_event_t : boost::noncopyable
      {
      private:
        typedef std::unique_ptr< next::abstract_event_data > abstract_event_data_ptr;

        send_event_t();

      public:
        send_event_t( ::next::widgets::dispatcher& d, abstract_event_data_ptr event_data_ptr )
          : event_data_( std::move( event_data_ptr ) )
          , d_( d )
        {

        }

        send_event_t( send_event_t && other )
          : event_data_( std::move( other.event_data_ ) )
          , d_( std::move( other.d_ ) )
        {
        }

        send_event_from_t< Event > from( widget& h )
        {
          return send_event_from_t< Event >( d_, h, std::move( event_data_ ) );
        }

      private:
        abstract_event_data_ptr     event_data_;
        next::widgets::dispatcher&  d_;
      };
    }

    class dispatcher : private next::dispatcher
    {
    public:

      template< typename... Args >
      dispatcher( Args&&... args )
        : next::dispatcher( args... )
      {
      }

      std::weak_ptr< thread_group > create_thread_group();

      template< typename Event, typename... Args >
      details::send_event_t< Event > send_event( Args && ... args )
      {
        return details::send_event_t< Event >( *this, std::make_unique< next::event_data< Event > >( std::forward< Args >( args )... ) );
      }

    private:
      template< typename Event >
      friend class details::send_event_from_t;

      void send_event_impl( widget& from, widget& to, std::unique_ptr< next::abstract_event_data > event_data );

    private:
      friend class widget;

      // TODO REFACTOR THIS.
      // event_handler should only care about create_thread_group method
      // create just an interface for this one and use it instead of requiring a dispatcher in the event_handler constructor
      next::dispatcher& get_base_dispatcher();
    };
  }
}