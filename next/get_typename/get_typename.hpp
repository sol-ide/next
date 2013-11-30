
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <next/get_typename/config.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <string>


namespace next
{
  namespace details
  {
    NEXT_GET_TYPENAME_EXPORT std::string get_typename_from_function_name( const char* function_name );

#ifdef _MSC_VER
    template< typename T >
    const std::string& get_typename_impl()
    {
      static const char* lpsz_function_name = __FUNCSIG__;
      static const std::string function_name = details::get_typename_from_function_name( lpsz_function_name );
      return function_name;
    }
#else
# ifdef __GNUC__
#   ifdef __clang__
    template< typename T >
    const std::string& get_typename_impl( typename std::remove_reference< T >::type* dummy = nullptr )
    {
      static const char* lpsz_function_name = __PRETTY_FUNCTION__;
      static const std::string function_name = details::get_typename_from_function_name( lpsz_function_name );
      return function_name;
    }
#   else
    template< typename T >
    const std::string& get_typename_impl()
    {
      static const char* lpsz_function_name = __PRETTY_FUNCTION__;
      std::cout << lpsz_function_name << std::endl;
      static const std::string function_name = details::get_typename_from_function_name( lpsz_function_name );
      return function_name;
    }
#   endif
# endif
#endif

    template< typename T >
    struct remove_cv
    {
      typedef T type;
    };

    template< typename T >
    struct remove_cv< volatile T >
    {
      typedef T type;
    };

    template< typename T >
    struct remove_cv< const T >
    {
      typedef T type;
    };

    template< typename T >
    struct remove_cv< const volatile T >
    {
      typedef T type;
    };

    template< typename T >
    struct remove_cv< volatile T& >
    {
      typedef T& type;
    };

    template< typename T >
    struct remove_cv< const T& >
    {
      typedef T& type;
    };

    template< typename T >
    struct remove_cv< const volatile T& >
    {
      typedef T& type;
    };

    template< typename T >
    struct remove_cv< volatile T* >
    {
      typedef T* type;
    };

    template< typename T >
    struct remove_cv< const T* >
    {
      typedef T* type;
    };

    template< typename T >
    struct remove_cv< const volatile T* >
    {
      typedef T* type;
    };

    template< typename T >
    struct is_const_ref_or_ptr : std::false_type
    {
    };

    template< typename T >
    struct is_const_ref_or_ptr< const T& > : std::true_type
    {
    };

    template< typename T >
    struct is_const_ref_or_ptr< const T* > : std::true_type
    {
    };

  }

  template< typename T >
  const std::string& get_typename()
  {
    // typedef typename details::remove_cv< typename std::remove_reference< T >::type >::type unqualified_type;
    typedef typename details::remove_cv< T >::type unqualified_type;
    return details::get_typename_impl< unqualified_type >();
  }

  namespace details
  {
    template< bool is_const >
    struct get_const_string;

    template<>
    struct get_const_string< true >
    {
      static std::string value()
      {
        return "const ";
      }
    };

    template<>
    struct get_const_string< false >
    {
      static std::string value()
      {
        return std::string();
      }
    };

    template< bool is_volatile >
    struct get_volatile_string;

    template<>
    struct get_volatile_string< true >
    {
      static std::string value()
      {
        return "volatile ";
      }
    };

    template<>
    struct get_volatile_string< false >
    {
      static std::string value()
      {
        return std::string();
      }
    };

    template< typename T >
    std::string construct_cv_type()
    {
      std::string typename_string = get_const_string< ( std::is_const< T >::value && ( std::is_pointer< T >::value == false ) ) || details::is_const_ref_or_ptr< T >::value >::value()
                                  + get_volatile_string< std::is_volatile< T >::value >::value()
                                  + get_typename< T >()
                                  + get_const_string < std::is_const< T >::value && std::is_pointer< T >::value >::value();
      boost::trim_right( typename_string );
      return typename_string;
    }
  }

  template< typename T >
  const std::string& get_cv_typename()
  {
    static const std::string cv_type( details::construct_cv_type< T >( ) );
    return cv_type;
  }
}
