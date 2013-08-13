
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <next/dispatcher.hpp>
#include <next/event/event_handler.hpp>
#include <next/event/event.hpp>

#include <iostream>

namespace mine
{
    struct an_event : public next::event < void( int ) >
    {

    };
}

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

