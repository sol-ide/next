
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <next/event/dispatcher.hpp>

namespace next
{
  namespace widgets
  {
    class widget;
  }

  extern template class dispatcher< next::widgets::widget >;

  namespace widgets
  {
    typedef next::dispatcher< next::widgets::widget > dispatcher; 
  }
}
