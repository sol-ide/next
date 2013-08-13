
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "event_handler.hpp"
#include "dispatcher.hpp"

namespace next
{
    event_handler_impl::event_handler_impl( next::dispatcher& d )
        : group_( d.create_thread_group() )
    {
    }

    event_handler_impl::event_handler_impl( const event_handler_impl& other )
        : group_( other.group_ )
    {
    }

    void event_handler_impl::call( const std::string& event_name, void* untyped_parameters )
    {
        auto slot_range = slots_.equal_range( event_name );
        std::for_each(
            slot_range.first,
            slot_range.second,
            [ & ]( slots_type::value_type& data )
            {
                data.second->call( untyped_parameters );
            }
        );
    }

    std::weak_ptr< thread_group > event_handler_impl::get_thread_group()
    {
        return group_;
    }
}
