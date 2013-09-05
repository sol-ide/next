# Copyright (C) Troy Straszheim
#
# Distributed under the Boost Software License, Version 1.0. 
# See accompanying file LICENSE_1_0.txt or copy at 
#   http://www.next.org/LICENSE_1_0.txt 
#
set(EXPAT_FIND_QUIETLY TRUE)

# Try to find the Expat library 
find_package(EXPAT)

next_external_report(Expat INCLUDE_DIR LIBRARIES)
  
