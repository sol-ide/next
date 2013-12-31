
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <next/event/config.hpp>
#include <next/event/abstract_event_data.hpp>
#include <boost/optional.hpp>
#include <mutex>

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable:4251)
#endif

namespace next
{
  namespace details
  {
    template< typename EventHandler >
    struct thread_group_event_data
    {
      typedef std::unique_ptr< next::abstract_event_data< EventHandler > > event_data_type;

      boost::optional< EventHandler > from;
      boost::optional< EventHandler > to;
      event_data_type                  event_data;

      thread_group_event_data()
        : event_data( nullptr )
      {
      }

      thread_group_event_data( boost::optional< EventHandler& > from_hanlder, EventHandler& to_handler, event_data_type&& data )
        : from( from_hanlder )
        , to( to_handler)
        , event_data( std::move( data ) )
      {
      }

      void operator=( thread_group_event_data && other )
      {
        std::swap( from, other.from );
        std::swap( to, other.to );
        std::swap( event_data, other.event_data );
      }

    private:
      thread_group_event_data( const thread_group_event_data& );
    };
  }

  template< typename EventHandler >
  class thread_group
  {
  public:

    ~thread_group()
    {
    }

    void store_event_data( boost::optional< EventHandler& > from, EventHandler& to, std::unique_ptr< next::abstract_event_data< EventHandler > > event_data )
    {
      std::unique_lock< std::mutex > lock( event_data_mutex_ );
      event_data_list_.emplace_back( from, to, std::move( event_data ) );
    }

    bool get_first_free_message( details::thread_group_event_data< EventHandler >& free_message )
    {
      std::unique_lock< std::mutex > lock( event_data_mutex_ );
      if( event_data_list_.empty() == false )
      {
        free_message = std::move( event_data_list_.front() );
        event_data_list_.pop_front();
        return true;
      }
      return false;
    }

    bool has_pending_messages() const
    {
      std::unique_lock< std::mutex > lock( event_data_mutex_ );
      return event_data_list_.empty() == false;
    }

  private:
        
    typedef std::deque < details::thread_group_event_data< EventHandler > > event_data_list_type;
    event_data_list_type event_data_list_;
    mutable std::mutex   event_data_mutex_;
  };
}

#ifdef _MSC_VER
# pragma warning(push)
#endif
