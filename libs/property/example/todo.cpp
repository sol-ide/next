// Copyright (c) 2013 Sylvain Oliver

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <next/event/dispatcher.hpp>
#include <next/event/event_handler.hpp>
#include <next/event/event.hpp>

namespace mine
{
  struct is_less_than_ten : next::event< bool( int ) >{};
}
void todo_remove_listener()
{
//[todo_remove_listener
  next::dispatcher d{ next::thread_pool_size_t{ 4 } };

  next::event_handler h{ d };

  next::listener_handle l =
    h.listen< mine::is_less_than_ten >(
    []( int x ) -> bool
    {
      return x < 10;
    }
  );

  h.remove_listener( l );
//]
}

void todo_send_source()
{
//[todo_send_source
  next::dispatcher d{ next::thread_pool_size_t{ 4 } };

  next::event_handler h_send{ d };

  next::event_handler h_receive{ d };

  h_receive.listen< mine::is_less_than_ten >(
    []( int x ) -> bool
    {
      return x < 10;
    }
  );

  d.send_event< mine::is_less_than_ten >( 5 ).from( h_send ).to( h_receive );
//]
}
