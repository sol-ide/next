#----------------------------------------------------------------------------
# CMake framework maintained by Denis Arnaud on git://github.com/pocb/boost
#----------------------------------------------------------------------------
#

set (COMPILING_NEXT_WIDGET)

next_add_library (widget
  widget.cpp
  dispatcher.cpp

  SHARED_COMPILE_FLAGS "-DNEXT_WIDGET_DYN_LINK=1"
  STATIC_COMPILE_FLAGS "-DNEXT_WIDGET_STATIC_LINK=1"

  DEPENDS next_event next_property
)
