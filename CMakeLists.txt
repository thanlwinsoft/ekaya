#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
#
# Copyright 2010 ThanLwinSoft.org
#
# This file is part of the Ekaya Input Method.
cmake_minimum_required(VERSION 2.8)

project(thanlwinsoft_ekaya)

enable_language(C CXX)

set(CPACK_GENERATOR "NSIS")
set(CPACK_SOURCE_GENERATOR "NSIS")
set(CPACK_PACKAGE_NAME "Ekaya")
set(CPACK_PACKAGE_VENDOR "ThanLwinSoft")
set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 1)
set(CPACK_PACKAGE_VERSION_MAJOR 4)
set(CPACK_SOURCE_STRIP_FILES false)
set(CPACK_NSIS_URL_INFO_ABOUT "http://www.thanlwinsoft.org/")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Input Method for Windows supporting KMN keyboards")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ThanLwinSoft.org/Ekaya)
set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/ekaya/COPYING)
# set(CPACK_PACKAGE_DESCRIPTION_FILE )
#CPack doesn't seem to be easy to control, so call NSIS directly for now
#include(CPack)

enable_testing()

add_subdirectory(../win-iconv win-iconv)
set(CPACK_GENERATOR "NSIS")
add_subdirectory(ekaya)
add_subdirectory(kmfltest)
add_subdirectory(winkmfl)

file(GLOB BUILD_FILES ${PROJECT_BINARY_DIR}/*)

# Attempting to create a target to wipe out the CMake build directory
# fails with permission problems, so need to do it by hand.
