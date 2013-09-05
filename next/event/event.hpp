
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <next/event/aggregator.hpp>

#include <boost/function_types/result_type.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/function_types/function_arity.hpp>
#include <boost/fusion/adapted/mpl.hpp>
#include <boost/fusion/include/mpl.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/optional.hpp>

#include <future>

namespace next
{
  namespace details
  {
    template< typename ReturnType >
    struct select_aggregator_type_from_return_type
    {
      typedef unique_aggregator< ReturnType > aggregator_type;
    };

    template<>
    struct select_aggregator_type_from_return_type< void >
    {
      typedef empty_aggregator< void > aggregator_type;
    };




    template< typename ReturnType >
    struct select_future_result_type
    {
      typedef std::future< boost::optional< ReturnType > > future_type;
      typedef std::promise< boost::optional< ReturnType > > promise_type;
    };

    template<>
    struct select_future_result_type< void >
    {
      typedef std::future< bool > future_type;
      typedef std::promise< bool > promise_type;
    };

  }

	template< typename Signature >
  struct event
    : details::select_aggregator_type_from_return_type< typename boost::function_types::result_type< Signature >::type >
    , details::select_future_result_type< typename boost::function_types::result_type< Signature >::type >
	{
		typedef Signature signature;
		typedef boost::function_types::function_arity< Signature > arity;
		typedef typename boost::fusion::result_of::as_vector <
			typename boost::function_types::parameter_types< Signature >::type
		> ::type parameter_types;
		typedef typename boost::function_types::result_type< Signature >::type return_type;

	};
}