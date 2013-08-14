#            Copyright (c) Dean Sylvain Oliver 2013.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file ../../LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

include_directories(${NEXT_SOURCE_DIR})

if (MSVC)
  add_definitions(-DCOMPILING_NEXT_EVENT)
endif()

add_library(
  event SHARED
  ../src/dispatcher.cpp
  ../src/event_handler.cpp
  ../src/message_handling_thread.cpp
)

target_link_libraries(
  event
  get_typename
)