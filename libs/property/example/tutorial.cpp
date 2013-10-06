// Copyright (c) 2013 Sylvain Oliver

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

//[tutorial_includes
#include <next/event/dispatcher.hpp>
#include <next/event/event_handler.hpp>
#include <next/event/event.hpp>
//]
void tutorial_create_dispatcher()
{
//[tutorial_create_dispatcher
  next::dispatcher d{ next::thread_pool_size_t{ 4 } }
//]
}

//[tutorial_declare_event
namespace mine
{
  struct is_less_than_ten : next::event< bool( int ) >{};
}
//]

void tutorial_listen_event()
{
//[tutorial_listen_event
  next::dispatcher d{ next::thread_pool_size_t{ 4 } };

  next::event_handler h{ d };

  h.listen< mine::is_less_than_ten >(
    []( int x ) -> bool
    {
      return x < 10;
    }
  );
//]
}

void tutorial_send_an_event()
{
//[tutorial_send_an_event
  next::dispatcher d{ next::thread_pool_size_t{ 4 } };

  next::event_handler h{ d };

  std::future< boost::optional< bool > > future_result = d.send_event< mine::is_less_than_ten >( 1 ).to( h );
//]
}

void tutorial_send_many_events()
{
  next::dispatcher d{ next::thread_pool_size_t{ 4 } };

  next::event_handler h{ d };

//[tutorial_send_many_events
  std::vector< std::future< boost::optional< bool > > > result_vector;
  std::size_t max = 20;

  for( std::size_t current = 0 ; current < max ; ++current )
  {
     result_vector.push_back( d.send_event< mine::is_less_than_ten >( 1 ).to( h ) );
  }

  for( std::size_t current = 0 ; current < max ; ++current )
  {
    boost::optional< bool >& optional_result = result_vector[ current ].get();
    if( optional_result )
    {
      std::cout << current << " is less than ten : " << ( *optional_result ? "true" : "false" ) << std::endl;
    }
    else
    {
      std::cout << "Event has not been received" << std::endl;
    }

  }
//]
}
