
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


int main( int argc, char* argv[] )
{
    std::cout << next::get_typename< mine::an_event >() << std::endl;

    return 0;
}
