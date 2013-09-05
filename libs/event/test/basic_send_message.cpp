
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_STATIC_LINK

#include <boost/test/unit_test.hpp> // for main, BOOST_CHECK, etc.

#include <next/event/dispatcher.hpp>
#include <next/event/event_handler.hpp>
#include <next/event/event.hpp>

#include <iostream>

namespace mine
{
    struct an_event : public next::event < void( int ) >
    {

    };

    struct an_increment_event : public next::event < int( int ) >
    {

    };
}

/*
int main( int argc, char* argv[] )
{
    next::dispatcher d{ next::thread_pool_size_t{ 4 } };

    next::event_handler h{ d };
    h.listen< mine::an_event >(
        [ ]( int x )
        {
            std::cout << "event received : " << x << std::endl;
        }
    );

    d.send_event< mine::an_event >( 10 ).to( h );
    d.send_event< mine::an_event >( 20 ).to( h );
    d.send_event< mine::an_event >( 30 ).to( h );
    d.send_event< mine::an_event >( 40 ).to( h );
    d.send_event< mine::an_event >( 50 ).to( h );
    d.send_event< mine::an_event >( 60 ).to( h );
    d.send_event< mine::an_event >( 70 ).to( h );
    d.send_event< mine::an_event >( 80 ).to( h );
    d.send_event< mine::an_event >( 90 ).to( h );

    return 0;
}
*/

// Unit test for user-defined data saving
void event_basic_send_event()
{
  next::dispatcher d{ next::thread_pool_size_t{ 4 } };

  next::event_handler h{ d };
  h.listen< mine::an_event >(
    [ ]( int x )
  {
    std::cout << "event received : " << x << std::endl;
  }
  );

  d.send_event< mine::an_event >( 10 ).to( h );
  d.send_event< mine::an_event >( 20 ).to( h );
  d.send_event< mine::an_event >( 30 ).to( h );
  d.send_event< mine::an_event >( 40 ).to( h );
  d.send_event< mine::an_event >( 50 ).to( h );
  d.send_event< mine::an_event >( 60 ).to( h );
  d.send_event< mine::an_event >( 70 ).to( h );
  d.send_event< mine::an_event >( 80 ).to( h );
  d.send_event< mine::an_event >( 90 ).to( h );
}

void event_basic_send_message_with_result()
{
  next::dispatcher d{ next::thread_pool_size_t{ 4 } };

  next::event_handler h{ d };
  h.listen< mine::an_event >(
    [ ]( int x )
  {
    std::cout << "event received : " << x << std::endl;
  }
  );

  h.listen< mine::an_increment_event >(
    [ ]( int x ) -> int
  {
    std::cout << "event received : " << x << std::endl;
    return x + 1;
  }
  );

  auto result1 = d.send_event< mine::an_increment_event >( 10 ).to( h );
  auto result2 = d.send_event< mine::an_increment_event >( 20 ).to( h );
  auto result3 = d.send_event< mine::an_increment_event >( 30 ).to( h );
  auto result4 = d.send_event< mine::an_increment_event >( 40 ).to( h );
  auto result5 = d.send_event< mine::an_increment_event >( 50 ).to( h );
  std::cout << *result1.get() << std::endl;
  std::cout << *result2.get() << std::endl;
  std::cout << *result3.get() << std::endl;
  std::cout << *result4.get() << std::endl;
  std::cout << *result5.get() << std::endl;
}

void multiple_handler_registered_on_event()
{
  next::dispatcher d{ next::thread_pool_size_t{ 4 } };

  next::event_handler h{ d };
  h.listen< mine::an_event >(
    [ ]( int x )
  {
    std::cout << "event received [1] : " << x << std::endl;
  }
  );

  h.listen< mine::an_event >(
    [ ]( int x )
  {
    std::cout << "event received [2] : " << x << std::endl;
  }
  );

  h.listen< mine::an_event >(
    [ ]( int x )
  {
    std::cout << "event received [3] : " << x << std::endl;
  }
  );

  d.send_event< mine::an_event >( 10 ).to( h );
  d.send_event< mine::an_event >( 20 ).to( h );
  d.send_event< mine::an_event >( 30 ).to( h );
}

void empty_test()
{
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
    = BOOST_TEST_SUITE( "event library test" );

  test->add( BOOST_TEST_CASE( event_basic_send_event ) );
  test->add( BOOST_TEST_CASE( event_basic_send_message_with_result ) );
  test->add( BOOST_TEST_CASE( multiple_handler_registered_on_event ) );
  

  return test;
}