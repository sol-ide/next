
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <next/event/dispatcher.hpp>
#include <next/event/message_handling_thread.hpp>
#include <next/event/abstract_event_data.hpp>
#include <next/event/message_handling_client.hpp>
#include <next/event/message_handling_thread.hxx>

template class next::message_handling_thread< next::event_handler >;
