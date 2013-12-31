#----------------------------------------------------------------------------
# CMake framework maintained by Denis Arnaud on git://github.com/pocb/boost
#----------------------------------------------------------------------------
#

set (COMPILING_NEXT_PROPERTY)

next_add_library (property
   abstract_property.cpp
   abstract_property_backend.cpp

   SHARED_COMPILE_FLAGS "-DNEXT_EVENT_DYN_LINK=1"
   STATIC_COMPILE_FLAGS "-DNEXT_EVENT_STATIC_LINK=1"

   DEPENDS next_get_typename
)
