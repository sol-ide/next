
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp> // for main, BOOST_CHECK, etc.

#include <next/get_typename/get_typename.hpp>

#include <iostream>

namespace mine
{
    struct an_event
    {

    };

    struct an_increment_event
    {

    };
}


void get_typename_of_a_struct()
{

  BOOST_REQUIRE_EQUAL( next::get_typename< mine::an_event >(), "mine::an_event" );
  BOOST_REQUIRE_EQUAL( next::get_typename< mine::an_increment_event >(), "mine::an_increment_event" );

  BOOST_REQUIRE_EQUAL( next::get_typename< const mine::an_event >(), "const mine::an_event" );
  BOOST_REQUIRE_EQUAL( next::get_typename< const mine::an_increment_event >(), "const mine::an_increment_event" );

  BOOST_REQUIRE_EQUAL( next::get_typename< volatile mine::an_event >( ), "volatile mine::an_event" );
  BOOST_REQUIRE_EQUAL( next::get_typename< volatile mine::an_increment_event >(), "volatile mine::an_increment_event" );

  BOOST_REQUIRE_EQUAL( next::get_typename< const volatile mine::an_event >( ), "volatile const mine::an_event" );
  BOOST_REQUIRE_EQUAL( next::get_typename< const volatile mine::an_increment_event >( ), "volatile const mine::an_increment_event" );

  // Bug in msvc...
#ifndef _MSC_VER
  BOOST_REQUIRE_EQUAL( next::get_typename< const mine::an_event& >(), "const mine::an_event" );
  BOOST_REQUIRE_EQUAL( next::get_typename< const mine::an_increment_event& >(), "const mine::an_increment_event" );
#endif
}

// Unit test program
boost::unit_test_framework::test_suite *
  init_unit_test_suite
  (
  int, // "argc" is unused
  char *[] // "argv" is unused
  )
{
  boost::unit_test_framework::test_suite * test
    = BOOST_TEST_SUITE( "get_typename library test" );

  test->add( BOOST_TEST_CASE( get_typename_of_a_struct ) );


  return test;
}
