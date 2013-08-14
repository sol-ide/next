#            Copyright (c) Dean Sylvain Oliver 2013.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file ../../LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

include_directories(${NEXT_SOURCE_DIR})

add_library(
  get_typename STATIC
  ../src/get_typename.cpp
)