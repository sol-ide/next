
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <next/property/property.hpp>
#include <next/property/property_manager.hpp>
#include <next/event/event_handler.hpp>
#include <next/widget/dispatcher.hpp>

#include <boost/fusion/include/as_vector.hpp>
#include <boost/fusion/functional/invocation/invoke.hpp>
#include <boost/mpl/transform.hpp>

namespace next
{
  namespace widgets
  {
    class widget;

    template< typename T >
    struct is_placeholder
    {
      typedef std::false_type type;
    };

    namespace target
    {
      template< typename Property >
      class property
      {
      public:
        property();

        typedef typename Property::value_type value_type;

        void assign( boost::optional< widget >& /* from */, widget& to );

        property( const property& other );

        property( property&& other );
        property& operator=( const property& );

        boost::optional< value_type > operator()() const;

        void operator()( value_type& value );
        void operator()( value_type&& value );

      private:
        widget* to_;
      };
    }

    template< typename Property >
    struct is_placeholder< next::widgets::target::property < Property > >
    {
      typedef std::true_type type;
    };

    template< typename Property >
    struct is_placeholder< const next::widgets::target::property < Property > >
    {
      typedef std::true_type type;
    };

    namespace source
    {
      template< typename Property >
      class property
      {
      public:
        typedef typename Property::value_type value_type;

        property();

        void assign( boost::optional< widget >& from , widget& /* to */ );

        property( const property& other );

        property( property&& other );

        property& operator=( const property& );

        boost::optional< value_type > operator()() const;

        void operator()( value_type& value );

      private:
        boost::optional< widget >* from_;
      };
    }

    template< typename Property >
    struct is_placeholder< next::widgets::source::property < Property > >
    {
      typedef std::true_type type;
    };

    template< typename Property >
    struct is_placeholder< const next::widgets::source::property < Property > >
    {
      typedef std::true_type type;
    };

    namespace details
    {

      template<typename T> struct remove_class { };

      template<typename C, typename R, typename... A>
      struct remove_class<R( C::* )( A... )> {
        typedef R( type )( A... );
      };

      template<typename C, typename R, typename... A>
      struct remove_class<R( C::* )( A... ) const> {
        typedef R( type )( A... );
      };

      template<typename C, typename R, typename... A>
      struct remove_class<R( C::* )( A... ) volatile> {
        typedef R( type )( A... );
      };

      template<typename C, typename R, typename... A>
      struct remove_class<R( C::* )( A... ) const volatile> {
        typedef R( type )( A... );
      };

      template< typename F >
      struct functor_has_operator
      {
        // Types "yes" and "no" are guaranteed to have different sizes,
        // specifically sizeof(yes) == 1 and sizeof(no) == 2.
        typedef char yes[ 1 ];
        typedef char no[ 2 ];

        template< typename U, U >
        struct type_check;

        template <typename X>
        static yes& test( type_check< decltype( &X::operator() ), &X::operator() >* );

        template <typename>
        static no& test( ... );

        // If the "sizeof" of the result of calling test<T>(0) would be equal to sizeof(yes),
        // the first overload worked and T has a nested type named foobar.
        static const bool value = sizeof( test<F>( nullptr ) ) == sizeof( yes );
      };

      template< typename Signature, bool has_operator >
      struct functor_parameters_types_impl;

      template< typename Functor >
      struct functor_parameters_types;

      template< typename Functor >
      struct functor_parameters_types_impl< Functor, true >
      {
        typedef typename functor_parameters_types<
          typename remove_class< decltype( &Functor::operator() ) >::type
        >::type type;
      };

      template< typename Signature >
      struct functor_parameters_types_impl< Signature, false >
      {
        typedef typename boost::fusion::result_of::as_vector <
          typename boost::function_types::parameter_types< Signature >::type
        >::type type;
      };

      template< typename Functor >
      struct functor_parameters_types
      {
        typedef typename functor_parameters_types_impl< Functor, functor_has_operator< Functor >::value >::type type;
        // typedef typename functor_parameters_types_impl< Functor, true >::type type;
      };

      template< typename T, typename IsPlaceholder >
      struct make_pointer_if_not_placeholder_impl;

      template< typename T >
      struct make_pointer_if_not_placeholder_impl< T, std::false_type >
      {
        typedef typename std::add_pointer<
          typename std::remove_reference< T >::type
        >::type type;
      };

      template< typename T >
      struct make_pointer_if_not_placeholder_impl< T, std::true_type >
      {
        typedef typename std::remove_const<
          typename std::remove_reference< T >::type
        >::type type;
      };

      template< typename T, typename IsPlaceholder >
      struct make_pointer_if_not_placeholder_impl;

      template< typename T >
      struct make_pointer_if_not_placeholder 
      {
        typedef typename make_pointer_if_not_placeholder_impl<
          T,
          typename is_placeholder<
            typename std::remove_reference< T >::type
          >::type
        >::type type;
      };

      template< typename Parameters >
      struct bind_parameters
      {
        bind_parameters( const Parameters& parameters, boost::optional< widget >& from, widget& to )
          : parameters_( parameters )
          , from_( from )
          , to_( to )
        {
        }

        bind_parameters( const bind_parameters& other )
          : parameters_( other.parameters_ )
          , from_( other.from_ )
          , to_( other.to_ )
        {
        }

        bind_parameters( bind_parameters&& other )
          : parameters_( std::move( other.parameters_ ) )
          , from_( other.from_ )
          , to_( other.to_ )
        {
        }

        template<typename State, typename T, typename IsPlaceholder, typename IsConvertibleToParameter >
        struct result_impl;

        template<typename State, typename T, typename IsPlaceholder >
        struct result_impl< State, T, IsPlaceholder, std::true_type >
        {
          typedef std::false_type is_a_placeholder;
          typedef typename boost::mpl::int_< State::value + 1 > type;
        };

        template<typename State, typename T >
        struct result_impl< State, T, std::true_type, std::false_type >
        {
          typedef std::true_type is_a_placeholder;
          typedef State type;
        };

        template< typename Sequence, typename N, typename AtIsPossible >
        struct protected_at_implementation
        {
          typedef typename boost::fusion::result_of::at<
            Sequence,
            N
          >::type type;
        };

        template< typename Sequence, typename N >
        struct protected_at_implementation< Sequence, N, boost::mpl::bool_< false > >
        {
          typedef boost::mpl::na type;
        };

        template< typename Sequence, typename N >
        struct protected_at
        {
          typedef typename protected_at_implementation<
            Sequence,
            N,
            typename boost::mpl::less<
              N,
              typename boost::fusion::result_of::size< Sequence >::type
            >::type
          >::type type;
        };
        
        template<typename State, typename T >
        struct result
        {
          typedef result_impl<
            State,
            T,
            typename is_placeholder< T >::type, // If T is a placeholder
            typename std::is_convertible<       // If T can be used for the current element
              typename std::remove_pointer< T >::type,
              // typename boost::fusion::result_of::at< Parameters, State >::type
              typename protected_at< Parameters, State >::type
            >::type
          > impl;

          typedef typename impl::type type;
          typedef typename impl::is_a_placeholder is_a_placeholder;
        };

        template< typename State, typename T >
        void get_value( std::true_type, T& t ) const
        {
          t.assign( from_, to_ );
        }

        template< typename State, typename T >
        void get_value( std::false_type, T& t ) const
        {
          t = boost::fusion::at< State >( parameters_ );
        }

        template<typename State, typename T>
        typename result< State, T >::type operator()( const State&, T& t ) const
        {
          typename result< State, T >::is_a_placeholder is_placeholder;
          get_value< State >( is_placeholder, t );
          return typename result< State, T >::type();
        }

        const Parameters& parameters_;
        boost::optional< widget >& from_;
        widget& to_;
      };

      template< typename Event, typename F >
      class bind_slot : public next::details::abstract_slot< next::widgets::widget >
      {
      public:

        bind_slot( F&& f )
          : f_( std::forward< F >( f ) )
        {
        }

        template< typename Function, typename Sequence >
        typename boost::fusion::result_of::invoke< Function, Sequence >::type
        invoke( Function&& f, Sequence& s )
        {
          return boost::fusion::detail::invoke_impl<
            typename boost::remove_reference< Function >::type, Sequence
          >::call( f, s );
        }


        template< typename ParametersType, typename ReturnType >
        void select_call( ParametersType& parameters, ReturnType* result )
        {
          // *result = boost::fusion::invoke< F, ParametersType >( f_, parameters );
          *result = invoke( f_, parameters );
        }

        template< typename ParametersType >
        void select_call( ParametersType& parameters, void* result )
        {
          // boost::fusion::invoke< F, ParametersType >( f_, parameters );
          invoke( f_, parameters );
        }

        virtual void call( boost::optional< widget >& from, widget& to, void* untyped_parameters, void* untyped_result )
        {
          typename Event::parameter_types* parameters = static_cast< typename Event::parameter_types* >( untyped_parameters );
          typename Event::return_type* result = static_cast< typename Event::return_type* >( untyped_result );

          typedef typename details::functor_parameters_types< F >::type wanted_parameters;
          typedef typename boost::mpl::transform< wanted_parameters, make_pointer_if_not_placeholder< boost::mpl::_1 > >::type wanted_parameters_pointers_view_type;
          typedef typename boost::fusion::result_of::as_vector< wanted_parameters_pointers_view_type >::type wanted_parameters_pointers_type;

          wanted_parameters_pointers_type wanted_parameters_pointer;

          boost::fusion::fold( wanted_parameters_pointer, boost::mpl::int_< 0 >(), bind_parameters < typename Event::parameter_types >( *parameters, from, to ) );

          select_call( wanted_parameters_pointer, result );
        }

      private:
        F f_;
      };

    }

    template< typename F >
    struct property : public next::property< F >
    {
      using typename next::property< F >::value_type;
      typedef next::event < void( value_type ) > change;
    };

    class widget : public next::properties_manager< widget >, public next::event_handler_base< widget >
    {
    public:
      using properties_manager< widget >::property;
      using properties_manager< widget >::has_property;
      using properties_manager< widget >::listen;
      using properties_manager< widget >::get_property_backend;

      using event_handler_base< widget >::listen;

      widget( next::widgets::dispatcher& d );
      virtual ~widget();

      template< typename Event, typename F >
      void bind( F&& f )
      {
        handler_->template listen< Event >( std::make_unique< details::bind_slot< Event, F > >( std::forward< F >( f ) ) );
      }

      widget& operator=( const widget& other );
    private:

      template< typename ConcreteManager >
      friend struct next::details::property_created;

      template< typename Property >
      void property_created( Property& prop )
      {
        prop.template listen< typename Property::value_type >( [&]( const typename Property::value_type& value ){
          dispatcher_->send_event< typename Property::change >( value ).to( *this );
        });
      }

    private:
      next::widgets::dispatcher* dispatcher_;
    };

    namespace target
    {
      template< typename Property >
      property< Property >::property()
        : to_( nullptr )
      {
      }

      template< typename Property >
      void property< Property >::assign( boost::optional< widget >& /* from */, widget& to )
      {
        to_ = &to;
      }

      template< typename Property >
      property< Property >::property( const property& other )
        : to_( other.to_ )
      {
      }

      template< typename Property >
      property< Property >::property( property&& other )
        : to_( std::move( other.to_ ) )
      {
      }
      
      template< typename Property >
      property< Property >& property< Property >::operator=( const property& other )
      {
        to_ = other.to_;
      }

      template< typename Property >
      boost::optional< typename property< Property >::value_type >
      property< Property >::operator()() const
      {
        return to_->property< Property >();
      }

      template< typename Property >
      void property< Property >::operator()( value_type& value )
      {
        to_->property< Property >( value );
      }

      template< typename Property >
      void property< Property >::operator()( value_type&& value )
      {
        to_->property< Property >( std::move( value ) );
      }
    }

    namespace source
    {
      template< typename Property >
      property< Property >::property()
        : from_( nullptr )
      {
      }

      template< typename Property >
      void property< Property >::assign( boost::optional< widget >& from , widget& /* to */ )
      {
        from_ = &from;
      }

      template< typename Property >
      property< Property >::property( const property& other )
        : from_( other.from_ )
      {
      }

      template< typename Property >
      property< Property >::property( property&& other )
        : from_( std::move( other.from_ ) )
      {
      }

      template< typename Property >
      property< Property >& property< Property >::operator=( const property& other )
      {
        from_ = other.from_;
      }

      template< typename Property >
      boost::optional< typename property< Property >::value_type >
      property< Property >::operator()() const
      {
        return (*from_)->property< Property >();
      }

      template< typename Property >
      void property< Property >::operator()( value_type& value )
      {
        (*from_)->property< Property >( value );
      }
    }
  }
}
