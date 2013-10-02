// Copyright (c) 2013 Sylvain Oliver

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

//[tutorial_includes
#include <next/get_typename/get_typename.hpp>
//]
void tutorial_get_typename()
{
//[tutorial_get_typename
  struct a_type
  {
  };

  BOOST_REQUIRE_EQUAL( next::get_typename< a_type >(), "a_type" );
  BOOST_REQUIRE_EQUAL( next::get_typename< const a_type >(), "a_type" );
  BOOST_REQUIRE_EQUAL( next::get_typename< volatile a_type >(), "a_type" );
  BOOST_REQUIRE_EQUAL( next::get_typename< const volatile a_type >(), "a_type" );
//]
}


void tutorial_get_cv_typename()
{
//[tutorial_get_cv_typename
  struct a_type
  {
  };

  BOOST_REQUIRE_EQUAL( next::get_typename< a_type >(), "a_type" );
  BOOST_REQUIRE_EQUAL( next::get_typename< const a_type >(), "const a_type" );
  BOOST_REQUIRE_EQUAL( next::get_typename< volatile a_type >(), "volatile a_type" );
  BOOST_REQUIRE_EQUAL( next::get_typename< const volatile a_type >(), "const volatile a_type" );
//]
}
