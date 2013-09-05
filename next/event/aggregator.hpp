
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/optional.hpp>

namespace next
{
  namespace details
  {
    template< typename T >
    struct empty_aggregator
    {
    public:
      typedef bool value_type;

      empty_aggregator()
        : value_( false )
      {
      }

      template< typename U >
      void operator()( U&& )
      {
        value_ = true;
      }

      value_type value() const
      {
        return value_;
      }

    private:
      value_type value_;
    };

    template< typename T >
    class unique_aggregator
    {
    public:
      typedef boost::optional< T > value_type;

      template< typename U >
      void operator( )( U&& value )
      {
        value_ = std::forward< U >( value );
      }

      value_type value() const
      {
        return value_;
      }

    private:
      value_type value_;
    };
  }
}