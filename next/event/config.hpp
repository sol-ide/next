#pragma once

#if defined(_WIN32)
# if NEXT_EVENT_DYN_LINK==1
#   if defined( NEXT_EVENT_SOURCE )
#     define NEXT_EVENT_EXPORT __declspec( dllexport )
#   else
#     define NEXT_EVENT_EXPORT __declspec( dllimport )
#   endif
# else
#   define NEXT_EVENT_EXPORT
# endif
#else
# define NEXT_EVENT_EXPORT
#endif