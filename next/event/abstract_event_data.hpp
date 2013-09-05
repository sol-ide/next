
//          Copyright Sylvain Oliver 2013 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "event_handler.hpp"
#include <boost/noncopyable.hpp>

namespace next
{

	class event_handler;

	class abstract_event_data : boost::noncopyable
	{
	public:
		abstract_event_data()
		{
		}

		virtual ~abstract_event_data()
		{

		}

		virtual void dispatch_message_to( event_handler& h ) = 0;

    virtual void get_future_result( void* value ) = 0;
  };

}