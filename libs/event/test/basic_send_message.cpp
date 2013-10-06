
//          Copyright Syvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_STATIC_LINK

#include <boost/test/unit_test.hpp> // for main, BOOST_CHECK, etc.


#include <next/event/dispatcher.hpp>
#include <next/event/event_handler.hpp>
#include <next/event/event.hpp>
#include <next/cpp/make_unique.hpp>
#include <thread>

#include <iostream>

#ifdef _MSC_VER
# include <intrin.h>
#endif

namespace mine
{
    struct an_event : public next::event < void( int ) >
    {

    };

    struct an_increment_event : public next::event < int( int ) >
    {

    };
}


// Unit test for user-defined data saving
void event_basic_send_event()
{
  std::mutex m;
  std::ostringstream oss;
  {
    next::dispatcher d{ next::thread_pool_size_t{ 4 } };

    next::event_handler h{ d };

    h.listen< mine::an_event >(
      [ &oss, &m ]( int x )
      {
        
        if( m.try_lock() )
        {
          oss << "event received : " << x << "\n";
          // std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
          m.unlock();
        }
        else
        {
          // __debugbreak();
          BOOST_FAIL( "two event are trtreated at the same times" );
        }
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
  BOOST_REQUIRE_EQUAL(
    oss.str(),
    "event received : 10\n"
    "event received : 20\n"
    "event received : 30\n"
    "event received : 40\n"
    "event received : 50\n"
    "event received : 60\n"
    "event received : 70\n"
    "event received : 80\n"
    "event received : 90\n"
    );
}

void event_basic_send_message_with_result()
{
  next::dispatcher d{ next::thread_pool_size_t{ 4 } };

  next::event_handler h{ d };
  auto operation = [ ]( int x ) -> int
  {
    return x + 1;
  };

  h.listen< mine::an_increment_event >(
    operation
  );

  std::deque< int > number_list{ 10, 20, 30, 40, 50 };
  std::deque< int > expected_result_list;

  std::transform(
    std::begin( number_list ),
    std::end( number_list ),
    std::back_inserter( expected_result_list ),
    operation
  );

  std::deque< std::future< boost::optional< int > > > future_result_list;
  std::deque< int >                                   result_list;
  

  for( int value : number_list )
  {
    future_result_list.push_back( d.send_event< mine::an_increment_event >( value ).to( h ) );
  }

  std::for_each(
    std::begin( future_result_list ),
    std::end( future_result_list ),
    [ &result_list ]( std::future < boost::optional< int > >& future_value )
    {
      const boost::optional< int >& value = future_value.get();
      if( value )
      {
        result_list.push_back( *value );
      }
    }
  );

  std::sort(
    std::begin( result_list ),
    std::end( result_list )
  );

  BOOST_REQUIRE_EQUAL( result_list.size(), expected_result_list.size() );
  BOOST_REQUIRE( std::equal( std::begin( result_list ), std::end( result_list ), std::begin( expected_result_list ) ) );
}

void multiple_handler_registered_on_event()
{
  int number_of_handler = 3;
  std::deque< int > send_values{ 10, 20, 30 };
  std::multimap< int, int > wanted_values;
  std::multimap< int, int > received_values;
  
  std::for_each(
    std::begin( send_values ),
    std::end( send_values ),
    [ &wanted_values, number_of_handler ]( int value )
    {
      for( int handler_index = 0; handler_index < number_of_handler; ++handler_index )
      {
        wanted_values.insert( std::make_pair( value, handler_index ) );
      }
    }
  );

  {
    next::dispatcher d{ next::thread_pool_size_t{ 4 } };

    next::event_handler h{ d };
    for( int handler_index = 0; handler_index < number_of_handler; ++handler_index )
    {
      h.listen< mine::an_event >(
        [ &received_values, handler_index ]( int value )
        {
          received_values.insert( std::make_pair( value, handler_index ) );
        }
      );
    }

    std::for_each(
      std::begin( send_values ),
      std::end( send_values ),
      [ &d, &h ]( int value )
      {
        d.send_event< mine::an_event >( value ).to( h );
      }
    );
  }

  BOOST_REQUIRE_EQUAL( received_values.size(), wanted_values.size() );
  BOOST_REQUIRE( std::equal( std::begin( received_values ), std::end( received_values ), std::begin( wanted_values ) ) );
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
