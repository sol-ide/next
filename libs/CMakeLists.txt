#
# Copyright Troy D. Straszheim
#
# Distributed under the Boost Software License, Version 1.0.
# See http://www.boost.org/LICENSE_1_0.txt
#
# Find each subdirectory containing a CMakeLists.txt file, and include
# it. This avoids the need to manually list which libraries in Next
# have CMakeLists.txt files.

# return a list of directories that we should add_subdirectory()
macro(NEXT_COLLECT_SUBPROJECT_DIRECTORY_NAMES dirname varname filename)
  file(GLOB globbed RELATIVE "${dirname}" "${dirname}/*/${filename}")
  foreach(file ${globbed})
    get_filename_component(dir ${file} PATH)
    set(${varname} ${${varname}} ${dir})
  endforeach()
endmacro()

# Find all of the subdirectories with .cmake files in them. These are
# the libraries with dependencies.
next_collect_subproject_directory_names(${NEXT_LIBS_PARENT_DIR} NEXT_MODULE_DIRS "module.cmake")
foreach(subdir ${NEXT_MODULE_DIRS})
  include("${NEXT_LIBS_PARENT_DIR}/${subdir}/module.cmake")
endforeach(subdir)

# Find all of the subdirectories with CMakeLists.txt files in
# them. This contains all of the Next libraries.
next_collect_subproject_directory_names(${NEXT_LIBS_PARENT_DIR} NEXT_SUBPROJECT_DIRS "CMakeLists.txt")

# Add all of the Next projects in reverse topological order, so that
# a library's dependencies show up before the library itself.
set(CPACK_INSTALL_CMAKE_COMPONENTS_ALL)
list(SORT NEXT_SUBPROJECT_DIRS)
topological_sort(NEXT_SUBPROJECT_DIRS NEXT_ _DEPENDS)

#
# Sanity-check for typos: all projects in BUILD_PROJECTS must exist
#
if ((NOT BUILD_PROJECTS STREQUAL "ALL") AND (NOT BUILD_PROJECTS STREQUAL "NONE"))
  foreach(project ${BUILD_PROJECTS})
    list(FIND NEXT_SUBPROJECT_DIRS ${project} THIS_SUBPROJECT_DIRS_INDEX)
    if (THIS_SUBPROJECT_DIRS_INDEX LESS 0)
      message(FATAL_ERROR "Nonexistant project \"${project}\" specified in BUILD_PROJECTS.  These project names should be all lowercase.")
    endif()
  endforeach()
endif()

set(NEXT_TEST_PROJECTS "" CACHE INTERNAL "hi" FORCE)

#
# include only directories of projects in BUILD_PROJECTS
#
message(STATUS "")
colormsg(_HIBLUE_ "Reading next project directories (per BUILD_PROJECTS)")
message(STATUS "")
set(NEXT_ALL_COMPONENTS "")

foreach(subdir ${NEXT_SUBPROJECT_DIRS})
  list(FIND BUILD_PROJECTS ${subdir} THIS_BUILD_PROJECTS_INDEX)
  if ((THIS_BUILD_PROJECTS_INDEX GREATER -1) OR (BUILD_PROJECTS STREQUAL "ALL"))
    message(STATUS "+ ${subdir}")
    add_subdirectory(${NEXT_LIBS_PARENT_DIR}/${subdir} ${CMAKE_BINARY_DIR}/libs/${subdir})
  endif()
endforeach()

set(NEXT_ALL_COMPONENTS ${NEXT_ALL_COMPONENTS} PARENT_SCOPE)

if(BUILD_TESTS AND NOT BUILD_TESTS STREQUAL "NONE")
  message(STATUS "")
  colormsg(_HIBLUE_ "Traversing project test directories (per BUILD_TESTS)")
  message(STATUS "")
else()
  message(STATUS "")
  colormsg(HIGRAY "BUILD_TESTS is NONE: skipping test directories.")
  message(STATUS "")
endif()

foreach(PROJ ${NEXT_TEST_PROJECTS})
  string(TOLOWER ${PROJ} proj)
  project(${proj})
  set(NEXT_PROJECT_NAME ${proj})
  foreach(dir ${NEXT_${PROJ}_TESTDIRS})
    message(STATUS "+ ${proj}")
    add_subdirectory(${dir} ${CMAKE_BINARY_DIR}/libs/${proj}/test)
  endforeach()
endforeach()

