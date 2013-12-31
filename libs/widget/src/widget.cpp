
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <next/widget/widget.hpp>
#include <next/event/event_handler.hpp>
#include <next/event/event_handler.hxx>
#include <next/event/message_handling_thread.hpp>
#include <next/event/message_handling_thread.hxx>

namespace next
{
  namespace widgets
  {
    widget::widget( next::widgets::dispatcher& d )
      : next::event_handler_base< widget >( d )
      , dispatcher_( &d )
    {
    }

    /* virtual */ widget::~widget()
    {
    }

    widget& widget::operator=( const widget& other )
    {
      dispatcher_ = other.dispatcher_;
      return *this;
    }
  }
}

template class next::event_handler_base< next::widgets::widget >;
template class next::event_handler_impl< next::widgets::widget >;
template class next::details::abstract_slot< next::widgets::widget >;
template class next::message_handling_thread< next::widgets::widget >;
