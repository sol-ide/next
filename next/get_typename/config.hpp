#pragma once

#if defined(_WIN32)
# if NEXT_GET_TYPENAME_DYN_LINK==1
#   if defined( NEXT_GET_TYPENAME_SOURCE )
#     define NEXT_GET_TYPENAME_EXPORT __declspec( dllexport )
#   else
#     define NEXT_GET_TYPENAME_EXPORT __declspec( dllimport )
#   endif
# else
#   define NEXT_GET_TYPENAME_EXPORT
# endif
#else
# define NEXT_GET_TYPENAME_EXPORT
#endif

#ifdef __GNUC__
#  ifdef __clang__
extern "C" char* gets (char* __s) __attribute__((deprecated));
#  endif
#endif
