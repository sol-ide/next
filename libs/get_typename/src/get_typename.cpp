
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <next/get_typename/get_typename.hpp>
#include <algorithm>
#include <iterator>

namespace next
{
	namespace details
	{
		std::string get_typename_from_function_name( const char* function_name )
		{
			const char* end_function_character = function_name + std::strlen( function_name ) + 1;

#ifdef _MSC_VER
			const std::size_t index_of_begin_name = std::strlen( "const class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > &__cdecl next::get_typename<" );
			const std::size_t index_of_end_name = std::strlen( function_name ) - std::strlen( ">(void)" );
#else
#  ifdef __GNUC__
#    ifdef __clang__
            const std::size_t index_of_begin_name = std::strlen( "const std::string &next::get_typename(" );
            const std::size_t index_of_end_name = std::strlen( function_name ) - std::strlen( " *)" );
#    else
            const std::size_t index_of_begin_name = std::strlen( "const string& next::get_typename() [with T = " );
            const std::size_t index_of_end_name = std::strlen( function_name ) - std::strlen( "; std::string = std::basic_string<char>]" );
#    endif
#  endif
#endif

			std::string typename_string( function_name + index_of_begin_name, function_name + index_of_end_name );

			static const char* typename_noise_list[] = { "const ", "volatile ", "struct ", "class " };
			for( std::size_t index = 0; index < sizeof( typename_noise_list ) / sizeof( *typename_noise_list ); ++index )
			{
				const char* typename_noise_item = typename_noise_list[ index ];
				boost::erase_first( typename_string, typename_noise_item );
			}

			return typename_string;
		}
	}
}