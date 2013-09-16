// Copyright (c) 2013 Sylvain Oliver

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <next/get_typename/get_typename.hpp>

void todo_get_cv_typename()
{
//[todo_get_cv_typename
  struct a_type
  {
  };

  BOOST_REQUIRE_EQUAL( next::get_typename< const a_type >(), "a_type" );
  BOOST_REQUIRE_EQUAL( next::get_cv_typename< const a_type >(), "const a_type" );
//]
}
