#            Copyright (c) Dean Sylvain Oliver 2013.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file ../../LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

include_directories(${NEXT_SOURCE_DIR})

# next_test_run( basic_typename DEPENDS boost_unit_test_framework )
next_test_run(
  basic_typename
  LINK_LIBS ${Boost_TEST_EXEC_MONITOR_LIBRARY}
  DEPENDS next_get_typename
)


