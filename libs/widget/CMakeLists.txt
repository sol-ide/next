#
# Copyright Sylvain Oliver
#
# Distributed under the Boost Software License, Version 1.0.
# See http://www.boost.org/LICENSE_1_0.txt
#

# Add a library target to the build system
next_library_project(
  widget
  SRCDIRS  src
  TESTDIRS test
  EXAMPLEDIRS example
  HEADERS  widget/widget.hpp widget
  DOCDIRS  doc
  DESCRIPTION  "The Next.Widget library is an implementation of blablaba"
  MODULARIZED
  AUTHORS  "Sylvain Oliver <oliver.sylvain -at- gmail.com>"
)


