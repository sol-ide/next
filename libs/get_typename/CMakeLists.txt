#
# Copyright Sylvain Oliver
#
# Distributed under the Boost Software License, Version 1.0.
# See http://www.boost.org/LICENSE_1_0.txt
#

# Add a library target to the build system
next_library_project(
  get_typename
  SRCDIRS  src
  TESTDIRS test
  HEADERS  get_typename/get_typename.hpp get_typename
  #  DOCDIRS 
  DESCRIPTION  "The Next.GetTypename library is an implementation of blablaba"
  MODULARIZED
  AUTHORS  "Sylvain Oliver <oliver.sylvain -at- gmail.com>"
)


