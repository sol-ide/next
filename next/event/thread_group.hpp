
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <next/event/config.hpp>
#include <next/event/abstract_event_data.hpp>
#include <boost/optional.hpp>
#include <mutex>

#pragma warning(push)
#pragma warning(disable:4251)

namespace next
{
  namespace details
  {
    struct thread_group_event_data
    {
      typedef std::unique_ptr< next::abstract_event_data > event_data_type;

      boost::optional< event_handler > handler;
      event_data_type                  event_data;

      thread_group_event_data()
        : event_data( nullptr )
      {
      }

      thread_group_event_data( event_handler& h, event_data_type&& data )
        : handler( h )
        , event_data( std::move( data ) )
      {
      }

      void operator=( thread_group_event_data && other )
      {
        std::swap( handler, other.handler );
        std::swap( event_data, other.event_data );
      }

    private:
      thread_group_event_data( const thread_group_event_data& );
    };
  }

  class NEXT_EVENT_EXPORT thread_group
  {
  public:

    ~thread_group()
    {
    }

    void store_event_data( event_handler& h, std::unique_ptr< next::abstract_event_data > event_data )
    {
      std::unique_lock< std::mutex > lock( event_data_mutex_ );
      event_data_list_.emplace_back( h, std::move( event_data ) );
    }

    bool get_first_free_message( details::thread_group_event_data& free_message )
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
        
    typedef std::deque < details::thread_group_event_data > event_data_list_type;
    event_data_list_type event_data_list_;
    mutable std::mutex   event_data_mutex_;
  };
}

#pragma warning(push)