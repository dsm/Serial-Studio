#
# Copyright (c) 2024 Alex Spataru <https://github.com/alex-spataru>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 17)

#-------------------------------------------------------------------------------
# Project information
#-------------------------------------------------------------------------------

set(PROJECT_DISPNAME            "Serial Studio")
set(PROJECT_EXECUTABLE          "Serial-Studio")
set(PROJECT_VENDOR              "Alex Spataru")
set(PROJECT_CONTACT             "serial-studio.github.io")
set(PROJECT_DESCRIPTION_SUMMARY "Multi-purpose data visualization software")
set(PROJECT_VERSION_MAJOR       "3")
set(PROJECT_VERSION_MINOR       "0")
set(PROJECT_VERSION_PATCH       "4")
set(PROJECT_VERSION             "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(PROJECT_APPCAST             "https://raw.githubusercontent.com/Serial-Studio/Serial-Studio/master/updates.json")

set(PROJECT_ROOT_DIR            ${CMAKE_CURRENT_LIST_DIR})
set(PROJECT_DESCRIPTION_FILE    "${PROJECT_ROOT_DIR}/README.md")
set(PROJECT_FILE_NAME           "${PROJECT_EXECUTABLE}-v${PROJECT_VERSION}")
set(PROJECT_FILE_LICENSE        "${PROJECT_ROOT_DIR}/LICENSE.md")

#-------------------------------------------------------------------------------
# Allow source code to access project information
#-------------------------------------------------------------------------------

add_definitions(-DPROJECT_VENDOR="${PROJECT_VENDOR}")
add_definitions(-DPROJECT_CONTACT="${PROJECT_CONTACT}")
add_definitions(-DPROJECT_VERSION="${PROJECT_VERSION}")
add_definitions(-DPROJECT_APPCAST="${PROJECT_APPCAST}")
add_definitions(-DPROJECT_DISPNAME="${PROJECT_DISPNAME}")

#-------------------------------------------------------------------------------
# Compiler flags
#-------------------------------------------------------------------------------

#
# MinGW-specific settings
#
if(WIN32 AND MINGW)
 add_compile_options(
  -O2                  # Optimize for speed
  -Wall                # Enable most warning messages
  -Wextra              # Enable additional warning messages
  -fuse-linker-plugin  # Use LTO plugin to enable link-time optimization
 )
 add_link_options(
  -Wl,--gc-sections    # Remove unused sections during linking
  -fuse-linker-plugin  # Enable LTO during linking
 )

 # Uncomment if needed for debugging
 # add_compile_options(-g -fsanitize=address)  # Enable debugging and Address Sanitizer
 # add_link_options(-fsanitize=address)        # Link with Address Sanitizer

#
# MSVC-specific settings
#
elseif(WIN32 AND MSVC)
 add_compile_options(
  /permissive-           # Enable strict ISO compliance, disable MSVC extensions
  /Zc:__cplusplus        # Correct __cplusplus value to reflect the C++ standard
  /Zc:preprocessor       # Enable standards-conforming preprocessor
  /MP                    # Multi-processor compilation for faster builds
  /vmg                   # Use general pointer-to-member representation
  /O2                    # Optimize for speed
  /W3                    # Warning level 3 (reasonable warnings without being too verbose)
  /GL                    # Enable whole program optimization
  /Zi                    # Generate debugging information in PDB files
  /MT                    # Use static runtime libraries
 )
 add_link_options(
  /OPT:REF               # Remove unreferenced functions/data
  /OPT:ICF               # Remove identical COMDATs to reduce binary size
  /LTCG                  # Link-time code generation for further optimization
  /DEBUG                 # Generate debugging information in the output binary
 )

 # Uncomment if needed for debugging with Address Sanitizer
 # add_compile_options(/fsanitize=address)    # Enable Address Sanitizer
 # add_link_options(/fsanitize=address)       # Link with Address Sanitizer

#
# macOS-specific settings
#
elseif(APPLE)
 add_compile_options(
  -O2                  # Optimize for speed
  -Wall                # Enable most warning messages
  -Wextra              # Enable additional warning messages
 )
 add_link_options(
  -Wl,-dead_strip      # Remove unused code and data during linking
 )
 # Uncomment if needed for debugging
 # add_compile_options(-g -fsanitize=address)  # Enable debugging and Address Sanitizer
 # add_link_options(-fsanitize=address)        # Link with Address Sanitizer

#
# Generic UNIX/Linux settings
#
elseif(UNIX)
 add_compile_options(
  -O2                  # Optimize for speed
  -Wall                # Enable most warning messages
  -Wextra              # Enable additional warning messages
 )
 add_link_options(
  -Wl,--gc-sections    # Remove unused sections during linking
 )

 # Uncomment if needed for debugging
 # add_compile_options(-g -fsanitize=address)  # Enable debugging and Address Sanitizer
 # add_link_options(-fsanitize=address)        # Link with Address Sanitizer
endif()

if (UNIX AND NOT APPLE)
 set(PROJECT_EXECUTABLE "serial-studio")
 set(CMAKE_C_COMPILER_AR "/usr/bin/ar")
 set(CMAKE_CXX_COMPILER_AR "/usr/bin/ar")
 set(CMAKE_C_COMPILER_RANLIB "/usr/bin/ranlib")
 set(CMAKE_CXX_COMPILER_RANLIB "/usr/bin/ranlib")
endif()

#-------------------------------------------------------------------------------
# Define project name & find Qt packages for correct CPack calls
#-------------------------------------------------------------------------------

project(Serial-Studio)

find_package(
 Qt6 REQUIRED
 COMPONENTS
 Core
 Qml
)

#-------------------------------------------------------------------------------
# Add subdirectories
#-------------------------------------------------------------------------------

add_subdirectory(lib)
add_subdirectory(app)
