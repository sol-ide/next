
//          Copyright Syvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_STATIC_LINK

#include <boost/test/unit_test.hpp> // for main, BOOST_CHECK, etc.


#include <next/property/property.hpp>
#include <next/property/property_manager.hpp>

#include <boost/optional.hpp>

namespace mine
{
  struct size : next::property< double >
  {
  };

}

void test_property_set_and_get()
{
  next::properties_manager<> m;

  m.property< mine::size >( 10.2 );
  boost::optional< mine::size::value_type > result = m.property< mine::size >();
  BOOST_REQUIRE( !!result );
  BOOST_REQUIRE_EQUAL( *result, 10.2 );
}

void test_property_existance()
{
  next::properties_manager<> m;
  BOOST_REQUIRE_EQUAL( m.has_property< mine::size >(), false );
  m.property< mine::size >( 0 );
  BOOST_REQUIRE_EQUAL( m.has_property< mine::size >( ), true );
}

void test_property_backend_callback()
{
  next::properties_manager<> m;
  m.property< mine::size >( 0 );
  boost::optional< next::property_backend< mine::size >& > backend = m.get_property_backend< mine::size >( );
  BOOST_REQUIRE( !!backend );

  boost::optional< mine::size::value_type > handled_value;
  backend->listen< mine::size >(
    [&]( mine::size::value_type value )
    {
      handled_value = value;
    }
  );

  BOOST_REQUIRE( !handled_value );

  m.property< mine::size >( 10 );
  BOOST_REQUIRE( !!handled_value );
  BOOST_REQUIRE_EQUAL( *handled_value, 10 );
}

void test_property_callback()
{
  next::properties_manager<> m;
  m.property< mine::size >( 0 );

  boost::optional< mine::size::value_type > handled_value;
  m.listen< mine::size >(
    [&]( const mine::size::value_type& value )
    {
      handled_value = value;
    }
  );
  boost::optional< next::property_backend< mine::size >& > backend = m.get_property_backend< mine::size >( );
  BOOST_REQUIRE( !!backend );

  BOOST_REQUIRE( !handled_value );
  backend->set( 10 );
  
  BOOST_REQUIRE( !!handled_value );
  BOOST_REQUIRE_EQUAL( *handled_value, 10 );

  boost::optional< mine::size::value_type > result = m.property< mine::size >();
  BOOST_REQUIRE( !!result );
  BOOST_REQUIRE_EQUAL( *result, 10 );
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

  test->add( BOOST_TEST_CASE( test_property_set_and_get ) );
  test->add( BOOST_TEST_CASE( test_property_existance ) );
  test->add( BOOST_TEST_CASE( test_property_backend_callback ) );
  test->add( BOOST_TEST_CASE( test_property_callback ) );
  
  
  

  return test;
}