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

# CMake parameter(s)
cmake_minimum_required(VERSION 2.8)

# Disable RPATH support for OS X
if    (${CMAKE_MAJOR_VERSION} EQUAL 3 AND APPLE)
  cmake_policy(SET CMP0042 OLD)
endif ()

# Project name
project(LaBGen)

# C++ flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Boost
find_package(Boost REQUIRED program_options)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

# OpenCV
find_package(OpenCV REQUIRED core highgui video imgproc features2d)
include_directories(SYSTEM ${OpenCV_INCLUDE_DIR})
add_definitions(-DOPENCV_VERSION=${OpenCV_VERSION_MAJOR})

# BGSLibrary
add_subdirectory(bgslibrary)
include_directories(bgslibrary)
file(MAKE_DIRECTORY ${LaBGen_BINARY_DIR}/main/config)

# Include directory.
include_directories(include)

# Recursion.
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(main)
