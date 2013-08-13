
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/function_types/result_type.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/function_types/function_arity.hpp>
#include <boost/fusion/adapted/mpl.hpp>
#include <boost/fusion/include/mpl.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/container/vector.hpp>

namespace next
{
	template< typename Signature >
	struct event
	{
		typedef Signature signature;
		typedef boost::function_types::function_arity< Signature > arity;
		typedef typename boost::fusion::result_of::as_vector <
			typename boost::function_types::parameter_types< Signature >::type
		> ::type parameter_types;
		typedef typename boost::function_types::result_type< Signature >::type return_type;
	};
}