#
# Copyright 2024 Perrauo
#
# Licensed under the Proprietary License, Version 1.0 (the "License");
# you may not use this file except in compliance with the License.
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.13...3.17)

project(maya-ue-pixelstreaming)

#------------------------------------------------------------------------------
# global options
#------------------------------------------------------------------------------
set(CMAKE_INSTALL_MESSAGE "NEVER")

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules
)

#------------------------------------------------------------------------------
# modules and definitions
#------------------------------------------------------------------------------
include(cmake/utils.cmake)
find_package(Maya 2025 REQUIRED)

find_package(UFE REQUIRED)
if(UFE_FOUND)
    message(STATUS "Building with UFE ${UFE_VERSION}.")
else()
    message(STATUS "UFE not found.")
endif()

if(MAYA_APP_VERSION VERSION_GREATER 2024)
    # Look for Qt6 in the Maya devkit.
    # The Qt6 archive in the Maya devkit contains everything needed for the normal cmake find_package.
    set(CMAKE_PREFIX_PATH "${MAYA_DEVKIT_LOCATION}/Qt")
    set(WANT_QT_VERSION 6.5)
    find_package(Qt6 ${WANT_QT_VERSION} COMPONENTS Core Widgets QUIET)
    if (Qt6_FOUND)
    else()
        message(WARNING "Could not find Qt ${WANT_QT_VERSION} in Maya devkit directory: ${MAYA_DEVKIT_LOCATION}. \
                        You must extract Qt.tar.gz.")
    endif()
else()
    message(WARNING "Maya ${MAYA_APP_VERSION} does not contain Qt6 in its devkit.")
endif()

#------------------------------------------------------------------------------
# compiler configuration
#------------------------------------------------------------------------------
include(cmake/compiler_config.cmake)

#------------------------------------------------------------------------------
# gulrak filesystem
#------------------------------------------------------------------------------
# include(cmake/gulrak.cmake)

#------------------------------------------------------------------------------
# test
#------------------------------------------------------------------------------
if (BUILD_TESTS)
    include(cmake/googletest.cmake)
    include(cmake/test.cmake)
    fetch_googletest()
    enable_testing()
    add_subdirectory(test)
endif()

#------------------------------------------------------------------------------
# lib
#------------------------------------------------------------------------------
add_subdirectory(lib)


#------------------------------------------------------------------------------
# plugin
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# scripts
#------------------------------------------------------------------------------
add_subdirectory(scripts)

#------------------------------------------------------------------------------
# install
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Maya module files
#------------------------------------------------------------------------------
if (IS_WINDOWS)
    configure_file("modules/mayaUePixelStreaming_Win.mod.template" ${PROJECT_BINARY_DIR}/mayaUePixelStreaming.mod)
else()
    configure_file("modules/mayaUePixelStreaming.mod.template" ${PROJECT_BINARY_DIR}/mayaUePixelStreaming.mod)
endif()

install(FILES ${PROJECT_BINARY_DIR}/mayaUePixelStreaming.mod DESTINATION ${CMAKE_INSTALL_PREFIX})