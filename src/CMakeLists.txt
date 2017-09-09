# Copyright - Benjamin Laugraud <blaugraud@ulg.ac.be> - 2017
# http://www.montefiore.ulg.ac.be/~blaugraud
# http://www.telecom.ulg.ac.be/labgen
#
# This file is part of LaBGen.
#
# LaBGen is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# LaBGen is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with LaBGen.  If not, see <http://www.gnu.org/licenses/>.
file(
  GLOB_RECURSE
  LaBGen_src
  *.cpp
)

# Shared library.
add_library(
  LaBGen_shared
  SHARED
  ${LaBGen_src}
  ${LaBGen_include}
)

set_target_properties(
  LaBGen_shared
  PROPERTIES
  OUTPUT_NAME
  LaBGen
)

target_link_libraries(
  LaBGen_shared
  ${Boost_LIBRARIES}
  ${OpenCV_LIBS}
  bgs
)

# Static library.
add_library(
  LaBGen_static
  STATIC
  ${LaBGen_src}
  ${LaBGen_include}
)

set_target_properties(
  LaBGen_static
  PROPERTIES
  OUTPUT_NAME
  LaBGen
)

target_link_libraries(
  LaBGen_static
  ${Boost_LIBRARIES}
  ${OpenCV_LIBS}
  bgs
)
