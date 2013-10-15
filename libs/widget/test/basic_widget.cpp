
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

}

void test_property_set()
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


// Unit test program
boost::unit_test_framework::test_suite *
  init_unit_test_suite
  (
  int, // "argc" is unused
  char *[] // "argv" is unused
  )
{
  boost::unit_test_framework::test_suite * test
    = BOOST_TEST_SUITE( "properties library test" );

  test->add( BOOST_TEST_CASE( test_property_set ) );
  
  
  

  return test;
}