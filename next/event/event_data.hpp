
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "abstract_event_data.hpp"
#include "get_typename.hpp"

namespace next
{
	template< typename Event >
	class event_data : public abstract_event_data
	{
	public:
		typedef typename Event::parameter_types parameters_type;

		template< typename... Args >
		event_data( Args&&... args )
			: parameters_( std::forward< Args >( args )... )
		{
		}

		virtual void dispatch_message_to( event_handler& h )
		{
			h.call( next::get_typename< Event >(), &parameters_ );
		}

	private:

		parameters_type parameters_;
	};
}