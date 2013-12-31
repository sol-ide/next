#----------------------------------------------------------------------------
# CMake framework maintained by Denis Arnaud on git://github.com/pocb/boost
#----------------------------------------------------------------------------
#

set (COMPILING_NEXT_EVENT)

next_add_library (event
  event_handler.cpp
  message_handling_thread.cpp

  SHARED_COMPILE_FLAGS "-DNEXT_EVENT_DYN_LINK=1"
  STATIC_COMPILE_FLAGS "-DNEXT_EVENT_STATIC_LINK=1"

  DEPENDS next_get_typename
)
