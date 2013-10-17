
//          Copyright Syvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_STATIC_LINK

#include <boost/test/unit_test.hpp> // for main, BOOST_CHECK, etc.

#include <next/event/dispatcher.hpp>
#include <next/widget/widget.hpp>

namespace mine
{
  struct size : public next::widgets::property< double >
  {
  };

  struct an_event : public next::event < void( double ) >
  {
  };
}

void test_property_set_from_backend()
{
  boost::optional< double > received_value;
  {
    next::dispatcher d{ next::thread_pool_size_t{ 4 } };
    next::widgets::widget w{ d };

    w.listen< mine::size::change >(
      [ &received_value ]( const double& value )
      {
        received_value = value;
      }
    );

    BOOST_REQUIRE_EQUAL( w.has_property< mine::size >(), false );
    w.property< mine::size >( 0.0 );
    BOOST_REQUIRE_EQUAL( w.has_property< mine::size >(), true );
    auto& backend = w.get_property_backend< mine::size >();
    BOOST_REQUIRE( !!backend );
    BOOST_REQUIRE( !received_value );
    backend->set( 10.0 );
  }
  BOOST_REQUIRE( !!received_value );
  BOOST_REQUIRE_EQUAL( *received_value, 10.0 );
}

void test_send_event()
{
  boost::optional< double > received_value;
  {
    next::dispatcher d{ next::thread_pool_size_t{ 4 } };
    next::widgets::widget w{ d };

    w.listen< mine::an_event >(
      [ &received_value ]( const double& value )
      {
        received_value = value;
      }
    );

    BOOST_REQUIRE( !received_value );
    d.send_event< mine::an_event >( 3.1415 ).to( w );
  }
  BOOST_REQUIRE( !!received_value );
  BOOST_REQUIRE_EQUAL( *received_value, 3.1415 );
}

void test_bind_event_with_property()
{
  {
    next::dispatcher d{ next::thread_pool_size_t{ 4 } };
    next::widgets::widget w_source{ d };
    next::widgets::widget w_target{ d };

    w.bind< mine::an_event >(
      []( next::widgets::target::property< mine::size >& target_property, next::widget::source::property< mine::size >& source_property )
      {
        target_property( source_property() );
      }
    );

    w_source.property< mine::size >( 3.1415 );
    
    BOOST_REQUIRE_EQUAL( w_source.has_property< mine::size >(), true );
    BOOST_REQUIRE_EQUAL( w_target.has_property< mine::size >(), false );
    BOOST_REQUIRE_EQUAL( w_target.property< mine::size >(), 3.1415 );

    d.send_event< mine::an_event >( 42. ).from( w_source ).to( w_target );
  }
  BOOST_REQUIRE_EQUAL( w_source.has_property< mine::size >(), true );
  BOOST_REQUIRE_EQUAL( w_target.has_property< mine::size >(), true );

  BOOST_REQUIRE_EQUAL( w_source.property< mine::size >(), 3.1415 );
  BOOST_REQUIRE_EQUAL( w_target.property< mine::size >(), 3.1415 );
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
    = BOOST_TEST_SUITE( "widget library test" );

  test->add( BOOST_TEST_CASE( test_property_set_from_backend ) );
  test->add( BOOST_TEST_CASE( test_send_event ) );
  
  
  

  return test;
}