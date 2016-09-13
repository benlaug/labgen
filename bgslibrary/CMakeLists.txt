# Copyright - Benjamin Laugraud <blaugraud@ulg.ac.be> - 2016
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

# C++ flags.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")

file(
  GLOB_RECURSE
  bgs_src
  *.cpp
)

file(
  GLOB_RECURSE
  bgs_include
  *.h
  *.i
)

add_library(
  bgs
  STATIC
  ${bgs_src}
  ${bgs_include}
)

target_link_libraries(
  bgs
  ${Boost_LIBRARIES}
  ${OpenCV_LIBS}
)
