
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <next/event/event_handler.hpp>
#include <next/event/event_handler.hxx>


template class next::details::abstract_slot< next::event_handler >;
template class next::event_handler_impl< next::event_handler >;
template class next::event_handler_base< next::event_handler >;
