#
# Copyright Troy D. Straszheim
#
# Distributed under the Boost Software License, Version 1.0.
# See http://www.boost.org/LICENSE_1_0.txt
#
if (BUILD_DOCUMENTATION_HTML)
  # Install style sheets and the main Next logo
  install(
    FILES
      boostbook.css
      docutils.css
      reference.css
      ../../next.png
      ${CMAKE_SOURCE_DIR}/libs/index.html
      ${CMAKE_SOURCE_DIR}/libs/libraries.htm
      ${CMAKE_SOURCE_DIR}/libs/maintainers.txt
    DESTINATION share/next-${NEXT_VERSION}/libs
  )


  # Install images
  install(DIRECTORY images
    DESTINATION share/next-${NEXT_VERSION}/libs
    COMPONENT Core
    PATTERN "CVS" EXCLUDE
    PATTERN ".svn" EXCLUDE)
endif ()

