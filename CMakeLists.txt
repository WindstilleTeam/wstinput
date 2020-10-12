# Windstille - A Sci-Fi Action-Adventure Game
# Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.15)
project(wstinput VERSION 0.3.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/external/cmake-modules/)

include(GNUInstallDirs)
include(ClangTidy)
include(MaximumWarnings)

find_package(PkgConfig REQUIRED)
pkg_search_module(SDL2 REQUIRED sdl2 IMPORTED_TARGET)

# Build dependencies
function(build_dependencies)
  # set(BUILD_TESTS OFF)
  # set(BUILD_EXTRA OFF)
  if(NOT TARGET prio)
    add_subdirectory(external/priocpp/ EXCLUDE_FROM_ALL)
  endif()
endfunction()
build_dependencies()

file(GLOB WSTINPUT_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  include/wstinput/*.hpp)
file(GLOB WSTINPUT_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  src/*.cpp)

add_library(wstinput STATIC ${WSTINPUT_SOURCES})
set_target_properties(wstinput PROPERTIES PUBLIC_HEADER "${WSTINPUT_HEADERS}")
target_compile_options(wstinput PUBLIC ${WARNINGS_CXX_FLAGS})
set_target_properties(wstinput PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF)
target_include_directories(wstinput SYSTEM PUBLIC ${OPENAL_INCLUDE_DIR})
target_include_directories(wstinput PUBLIC include/)
target_include_directories(wstinput PRIVATE src/ include/wstinput/)
target_link_libraries(wstinput PUBLIC
  prio
  PkgConfig::SDL2)

install(TARGETS wstinput
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")

# EOF #