#----------------------------------------------------------------------------
# CMake framework maintained by Denis Arnaud on git://github.com/pocb/boost
#----------------------------------------------------------------------------
#

set (COMPILING_NEXT_EVENT)

next_add_library (get_typename
  get_typename.cpp

  SHARED_COMPILE_FLAGS "-DNEXT_GET_TYPENAME_DYN_LINK=1"
  STATIC_COMPILE_FLAGS "-DNEXT_GET_TYPENAME_STATIC_LINK=1"
)
