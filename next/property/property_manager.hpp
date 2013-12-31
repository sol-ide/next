
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <next/property/abstract_property.hpp>
#include <next/cpp/make_unique.hpp>
#include <boost/optional.hpp>
#include <boost/none_t.hpp>
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>

namespace next
{
  template< typename ConcreteManager = boost::none_t >
  class properties_manager_impl
  {
  public:
    template< typename Property >
    boost::optional< typename Property::value_type > property() const;

    template< typename Property, typename Manager >
    void property( const typename Property::value_type& value, Manager&& self );

    template< typename Property >
    bool has_property() const;

    template< typename Property >
    void listen( const std::function < void ( const typename Property::value_type& ) >& f );

    template< typename Property >
    boost::optional< property_backend< Property >& > get_property_backend() const;

  private:
    typedef std::unordered_map< std::string, std::unique_ptr< abstract_property > > property_list_type;
    property_list_type properties_;
    mutable std::mutex properties_mutex_;
  };

  namespace
  {
    template< typename Property, typename ConcreteManager >
    struct set_property
    {
      template< typename Self, typename Implementation, typename ValueType >
      static void call( Self& self, Implementation& impl, ValueType&& value )
      {
#if defined( __GNUC__ ) && !defined( __clang__ )
        typedef void (Implementation::* ptr)( ValueType, ConcreteManager& );
        ptr p = &Implementation::template property< Property >;

        ( impl.*p )(
          std::forward< ValueType >( value ),
          static_cast< ConcreteManager& >( self )
        );
#else
        impl.template property< Property >(
          std::forward< ValueType >( value ),
          static_cast< ConcreteManager& >( self )
        );
#endif
      }
    };

    template< typename Property >
    struct set_property< Property, boost::none_t >
    {
      template< typename Self, typename Implementation, typename ValueType >
      static void call( Self& self, Implementation& impl, ValueType&& value )
      {
#if defined( __GNUC__) && !defined(__clang__)
        typedef void (Implementation::* ptr)( ValueType, boost::none_t&& );
        ptr p = &Implementation::template property< Property >;

        ( impl.*p )(
          std::forward< ValueType >( value ),
          boost::none_t{}
        );
#else   
        impl.template property< Property >(
          std::forward< ValueType >( value ),
          boost::none_t{}
        );
#endif
      }
    };
  }

  template< typename ConcreteManager = boost::none_t >
  class properties_manager
  {
  public:
    properties_manager()
      : impl_( std::make_unique< properties_manager_impl< ConcreteManager > >() )
    {
    }

    template< typename Property >
    boost::optional< typename Property::value_type > property() const
    {
      return impl_->template property< Property >();
    }

    template< typename Property >
    void property( const typename Property::value_type& value )
    {
      set_property< Property, ConcreteManager >::call( *this, *impl_.get(), value );
      // impl_->property< Property >( value );
    }

    template< typename Property >
    bool has_property() const
    {
      return impl_->template has_property< Property >();
    }

    template< typename Property >
    void listen( const std::function < void ( const typename Property::value_type& ) >& f )
    {
      impl_->template listen< Property >( f );
    }

    template< typename Property >
    boost::optional< property_backend< Property >& > get_property_backend() const
    {
      return impl_->template get_property_backend< Property >();
    }

  private:
    std::shared_ptr< properties_manager_impl< ConcreteManager > > impl_;
  };
}

#include <next/property/property_manager.hxx>
