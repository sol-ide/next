#pragma once

#if defined(_WIN32)
#if defined( COMPILING_NEXT_EVENT )
  #define NEXT_EVENT_EXPORT __declspec( dllexport )
 #else
  #define NEXT_EVENT_EXPORT __declspec( dllimport )
 #endif
#else
  #define NEXT_EVENT_EXPORT
#endif