
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/algorithm/string/erase.hpp>
#include <string>


namespace next
{
	namespace details
	{
		std::string get_typename_from_function_name( const char* function_name );
	}

#ifdef _MSC_VER
	template< typename T >
	const std::string& get_typename()
	{
		static const char* lpsz_function_name = __FUNCSIG__;
		static const std::string function_name = details::get_typename_from_function_name( lpsz_function_name );
		return function_name;
	}
#else
# ifdef __GNUC__
#   ifdef __clang__
    template< typename T >
    const std::string& get_typename( T* dummy = NULL )
    {
        static const char* lpsz_function_name = __PRETTY_FUNCTION__;
        static const std::string function_name = details::get_typename_from_function_name( lpsz_function_name );
        return function_name;
    }
#   else
    template< typename T >
    const std::string& get_typename()
    {
        static const char* lpsz_function_name = __PRETTY_FUNCTION__;
        std::cout << lpsz_function_name << std::endl;
        static const std::string function_name = details::get_typename_from_function_name( lpsz_function_name );
        return function_name;
    }
#   endif
# endif
#endif
}