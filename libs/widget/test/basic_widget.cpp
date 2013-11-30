
//          Copyright Syvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_STATIC_LINK

#include <boost/test/unit_test.hpp> // for main, BOOST_CHECK, etc.

#include <next/widget/dispatcher.hpp>
#include <next/widget/widget.hpp>

#include <boost/thread/barrier.hpp>

namespace mine
{
  struct size : public next::widgets::property< double >
  {
  };

  struct an_event : public next::event < void( double ) >
  {
  };

  struct a_signal : public next::event < void() >
  {
  };

}

void test_property_set_from_backend()
{
  boost::optional< double > received_value;
  {
    next::widgets::dispatcher d{ next::thread_pool_size_t{ 4 } };
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
    next::widgets::dispatcher d{ next::thread_pool_size_t{ 4 } };
    next::widgets::widget source{ d };
    next::widgets::widget dest{ d };

    dest.listen< mine::an_event >(
      [ &received_value ]( const double& value )
      {
        received_value = value;
      }
    );

    BOOST_REQUIRE( !received_value );
    d.send_event< mine::an_event >( 3.1415 ).from( source ).to( dest );
  }
  BOOST_REQUIRE( !!received_value );
  BOOST_REQUIRE_EQUAL( *received_value, 3.1415 );
}

void test_bind_event_with_property()
{
  {
    next::widgets::dispatcher d{ next::thread_pool_size_t{ 4 } };
    next::widgets::widget w_source{ d };
    next::widgets::widget w_target{ d };

    boost::barrier b{ 2 };

    w_target.bind< mine::a_signal >(
      [&]( next::widgets::target::property< mine::size >& target_property, next::widgets::source::property< mine::size >& source_property )
      {
        target_property( *source_property() );
        b.wait();
      }
    );

    w_source.property< mine::size >( 3.1415 );

    BOOST_REQUIRE_EQUAL( w_source.has_property< mine::size >(), true );
    BOOST_REQUIRE_EQUAL( w_target.has_property< mine::size >(), false );
    BOOST_REQUIRE_EQUAL( w_target.property< mine::size >(), 3.1415 );

    d.send_event< mine::a_signal >().from( w_source ).to( w_target );
    b.wait();

    BOOST_REQUIRE_EQUAL( w_source.has_property< mine::size >(), true );
    BOOST_REQUIRE_EQUAL( w_target.has_property< mine::size >(), true );

    BOOST_REQUIRE_EQUAL( w_source.property< mine::size >(), 3.1415 );
    BOOST_REQUIRE_EQUAL( w_target.property< mine::size >(), 3.1415 );
  }
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