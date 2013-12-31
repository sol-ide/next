#            Copyright (c) Dean Sylvain Oliver 2013.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file ../../LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

include_directories(${NEXT_SOURCE_DIR})


next_test_run(
  basic_send_message
  LINK_LIBS ${Boost_TEST_EXEC_MONITOR_LIBRARY}
            ${Boost_SYSTEM_LIBRARY}
  DEPENDS next_event
)
