#            Copyright (c) Dean Sylvain Oliver 2013.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file ../LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 2.8)
project(Next)

list(APPEND CMAKE_MODULE_PATH ${Next_SOURCE_DIR}/tools/build/CMake)
include(NextTesting)




include(NextUtils)

message(STATUS "")
colormsg(_HIBLUE_ "Next.CMake starting")

##########################################################################
# Version information                                                    #
##########################################################################

# We parse the version information from the next/version.hpp header.
file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/next/version.hpp NEXT_VERSIONSTR
  REGEX "#define[ ]+NEXT_VERSION[ ]+[0-9]+")
string(REGEX MATCH "[0-9]+" NEXT_VERSIONSTR ${NEXT_VERSIONSTR})
if (NEXT_VERSIONSTR)
  math(EXPR NEXT_VERSION_MAJOR "${NEXT_VERSIONSTR} / 100000")
  math(EXPR NEXT_VERSION_MINOR "${NEXT_VERSIONSTR} / 100 % 1000")
  math(EXPR NEXT_VERSION_SUBMINOR "${NEXT_VERSIONSTR} % 100")
  set(NEXT_VERSION "${NEXT_VERSION_MAJOR}.${NEXT_VERSION_MINOR}.${NEXT_VERSION_SUBMINOR}")
else()
  message(FATAL_ERROR
    "Unable to parse Next version from ${CMAKE_CURRENT_SOURCE_DIR}/next/version.hpp")
endif()

#
#  For intermittent deployment of docs
#
set(NEXT_CMAKE_HOST sodium.resophonic.com)
set(NEXT_CMAKE_DOCROOT /var/www/htdocs/next/)
set(NEXT_CMAKE_VERSIONED_DOCROOT ${NEXT_CMAKE_DOCROOT}/${NEXT_CMAKE_VERSION})
set(NEXT_CMAKE_URL ${NEXT_CMAKE_HOST}:${NEXT_CMAKE_VERSIONED_DOCROOT})

#
# Make sure that we reconfigure when next/version.hpp changes.
#
configure_file(next/version.hpp
   ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/version.stamp)

##########################################################################

# Put the libaries and binaries that get built into directories at the
# top of the build tree rather than in hard-to-find leaf
# directories. This simplifies manual testing and the use of the build
# tree rather than installed Next libraries.
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

message(STATUS "")
next_report_pretty("Next.CMake version" NEXT_CMAKE_VERSION)

include(NextConfig)
include(NextCore)
include(NextExternals)
include(NextDocs)
include(NextTesting)
##########################################################################

##########################################################################
# Build Features and Variants                                            #
##########################################################################


# User-level options deciding which variants we will build.
option(ENABLE_STATIC "Whether to build static libraries" ON)
option(ENABLE_SHARED "Whether to build shared libraries" ON)
option(ENABLE_DEBUG "Whether to build debugging libraries" ON)
option(ENABLE_RELEASE "Whether to build release libraries" ON)
option(ENABLE_SINGLE_THREADED "Whether to build single-threaded libraries" OFF)
option(ENABLE_MULTI_THREADED "Whether to build multi-threaded libraries" ON)

if(BUILD_VERSIONED)
  message(FATAL_ERROR "Option 'BUILD_VERSIONED' has changed, the new name is WINMANGLE_LIBNAMES")
endif(BUILD_VERSIONED)

#if(BUILD_TESTING)
#  message(FATAL_ERROR "Option 'BUILD_TESTING' is gone, new name is BUILD_TESTS, see the docs")
#endif()

option(WINMANGLE_LIBNAMES
  "mangle toolset and next version tags to into library names"
  ${WIN32})

option(BUILD_SOVERSIONED "Create libraries with SONAMES" ${UNIX})

if(UNIX)
  option(INSTALL_VERSIONED "Install to versioned directories" ON)
endif()


# the default set of library variants that we will be building
next_add_default_variant(RELEASE DEBUG)
next_add_default_variant(SHARED STATIC)
next_add_default_variant(MULTI_THREADED SINGLE_THREADED)

if (MSVC)
  # For now, we only actually support static/dynamic run-time variants for
  # Visual C++. Provide both options for Visual C++ users, but just fix
  # the values of the variables for all other platforms.
  option(ENABLE_STATIC_RUNTIME
    "Whether to build libraries linking against the static runtime"
    ON)

  option(ENABLE_DYNAMIC_RUNTIME
    "Whether to build libraries linking against the dynamic runtime"
    ON)

  next_add_default_variant(DYNAMIC_RUNTIME STATIC_RUNTIME)
endif()

##########################################################################
# Installation                                                           #
##########################################################################
if (WIN32)
  set(sep "_")
else()
  set(sep ".")
endif()

if(INSTALL_VERSIONED)
  set(verdir "next-${NEXT_VERSION_MAJOR}${sep}${NEXT_VERSION_MINOR}${sep}${NEXT_VERSION_SUBMINOR}")
else()
  set(verstring "")
endif()

set(NEXT_INCLUDE_INSTALL_DIR
  "include/${verdir}"
  CACHE STRING "Destination path under CMAKE_INSTALL_PREFIX for header files"
  )

set(NEXT_LIB_INSTALL_DIR
  "lib${LIB_SUFFIX}/${verdir}"
  CACHE STRING "Destination path under CMAKE_INSTALL_PREFIX for libraries"
  )

next_report_pretty("Install prefix" CMAKE_INSTALL_PREFIX)
next_report_pretty("Install include dir" NEXT_INCLUDE_INSTALL_DIR)
next_report_pretty("Install lib dir" NEXT_LIB_INSTALL_DIR)




install(DIRECTORY next
  DESTINATION ${NEXT_INCLUDE_INSTALL_DIR}
  PATTERN "CVS" EXCLUDE
  PATTERN ".svn" EXCLUDE)

#set(Boost_DEBUG 1)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTI_THREADED ON)

# I use the boost trunk on win32...
if(WIN32)
  find_package( Boost REQUIRED unit_test_framework )
else(WIN32)
  find_package( Boost REQUIRED test_exec_monitor system )
endif(WIN32)

set(CMAKE_VERBOSE_MAKEFILE true)

if (${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories   (${Boost_LIBRARY_DIRS})
endif(Boost_FOUND)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif()



##########################################################################
# Binary packages                                                        #
##########################################################################
#
#  CPACK_PACKAGE_NAME may not contain spaces when generating rpms
#
set(CPACK_PACKAGE_NAME "Next")
set(CPACK_PACKAGE_VENDOR "Next.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Next ${NEXT_VERSION} prerelease")
set(CPACK_PACKAGE_FILE_NAME "next-${NEXT_VERSION}-${NEXT_PLATFORM}-${NEXT_TOOLSET}")

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
  message(STATUS "Using generic cpack package description file.")
  set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
  set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
endif ()

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE_1_0.txt")
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Welcome.txt")
  message(STATUS "Using generic cpack welcome file.")
  set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_CURRENT_SOURCE_DIR}/Welcome.txt")
endif()

set(CPACK_PACKAGE_VERSION "${NEXT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${NEXT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${NEXT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${NEXT_VERSION_SUBMINOR}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Next")

if(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  # NOTE: No Next icon yet
  set(CPACK_MONOLITHIC_INSTALL ON) # don't be modular for now
  set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/tools/build/CMake\\\\Boost.bmp")
#  set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
  set(CPACK_NSIS_DISPLAY_NAME "Next ${NEXT_VERSION_MAJOR}.${NEXT_VERSION_MINOR}.${NEXT_VERSION_SUBMINOR} prerelease")
  set(CPACK_NSIS_HELP_LINK "https:\\\\\\\\:github.com\\sol-ide\\next")
  set(CPACK_NSIS_URL_INFO_ABOUT "https:\\\\\\\\:github.com\\sol-ide\\next")
  #set(CPACK_NSIS_CONTACT "oliver.sylvain@gmail.com")
  set(CPACK_NSIS_MODIFY_PATH ON)

  # Encode the compiler name in the package
  if (MSVC60)
    set(CPACK_PACKAGE_FILE_NAME "Next-${NEXT_VERSION}-vc6")
    set(CPACK_NSIS_DISPLAY_NAME "${CPACK_NSIS_DISPLAY_NAME} for Microsoft Visual C++ 6")
  elseif (MSVC70)
    set(CPACK_PACKAGE_FILE_NAME "Next-${NEXT_VERSION}-vc7")
    set(CPACK_NSIS_DISPLAY_NAME "${CPACK_NSIS_DISPLAY_NAME} for Microsoft Visual Studio 2002")
  elseif (MSVC71)
    set(CPACK_PACKAGE_FILE_NAME "Next-${NEXT_VERSION}-vc71")
    set(CPACK_NSIS_DISPLAY_NAME "${CPACK_NSIS_DISPLAY_NAME} for Microsoft Visual Studio 2003")
  elseif (MSVC80)
    set(CPACK_PACKAGE_FILE_NAME "Next-${NEXT_VERSION}-vc8")
    set(CPACK_NSIS_DISPLAY_NAME "${CPACK_NSIS_DISPLAY_NAME} for Microsoft Visual Studio 2005")
  elseif (MSVC90)
    set(CPACK_PACKAGE_FILE_NAME "Next-${NEXT_VERSION}-vc9")
    set(CPACK_NSIS_DISPLAY_NAME "${CPACK_NSIS_DISPLAY_NAME} for Microsoft Visual Studio 2008")
  elseif (BORLAND)
    set(CPACK_PACKAGE_FILE_NAME "Next-${NEXT_VERSION}-borland")
    set(CPACK_NSIS_DISPLAY_NAME "${CPACK_NSIS_DISPLAY_NAME} for Borland C++ Builder")
  endif (MSVC60)
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_NSIS_DISPLAY_NAME}")
endif(WIN32 AND NOT UNIX)
include(CPack)


##########################################################################
# Building Next libraries                                               #
##########################################################################
# Always include the directory where Next's include files will be.
if (TEST_INSTALLED_TREE)
  # Use the headers from the installation directory
  include_directories("${CMAKE_INSTALL_PREFIX}/${NEXT_HEADER_DIR}")
elseif (NEXT_CMAKE_SELFTEST)
  # Use selftest headers
  include_directories("${NEXT_CMAKE_SELFTEST_ROOT}/include")
  # and regular next headers
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})

else()
  # Use the headers directly from the Next source tree (in next/)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
endif (TEST_INSTALLED_TREE)

# Next.Build version 2 does this due to trouble with autolinking
# during building and testing.
# TODO: See if we can actually use auto-linking in our regression tests.
add_definitions(-DNEXT_ALL_NO_LIB=1)

#
# Get build space set up for exports file
#
set(NEXT_EXPORTS_FILE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Exports.cmake
  CACHE FILEPATH "File to export targets from next build directory")

file(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
file(REMOVE ${NEXT_EXPORTS_FILE})

if(NOT INSTALL_VERSIONED)
  set(pathelem "next/")
endif()

if (NOT NEXT_EXPORTS_INSTALL_DIR)
  set(NEXT_EXPORTS_INSTALL_DIR ${NEXT_LIB_INSTALL_DIR}
    CACHE STRING "Install location for cmake exports")
endif()

mark_as_advanced(NEXT_EXPORTS_FILE)

if( CMAKE_CXX_COMPILER MATCHES ".*clang" )
  set( CMAKE_COMPILER_IS_CLANGXX 1 )
endif()

if( CMAKE_COMPILER_IS_GNUCXX )
  add_definitions(-std=c++0x)
elseif( CMAKE_COMPILER_IS_CLANGXX )
  add_definitions(-std=c++11)
endif( CMAKE_COMPILER_IS_GNUCXX )

# Add build rules for documentation
add_subdirectory(doc)

# Add build rules for all of the Next libraries
add_subdirectory(libs)

# Add build rules for all of the Next tools
# TODO: On hold while I work on the modularity code
# add_subdirectory(tools)
##########################################################################

install(EXPORT Next DESTINATION ${NEXT_EXPORTS_INSTALL_DIR})

# add_subdirectory(tools/build/CMake)
