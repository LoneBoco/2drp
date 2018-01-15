cmake_minimum_required(VERSION 2.8.12)
project (bzip2 C)
set(PACKAGE "bzip2" CACHE STRING "")
##
# Configuration for post-install RPath
# Adapted from http://www.cmake.org/Wiki/CMake_RPATH_handling
##
IF(NOT MSVC)
  # use, i.e. don't skip the full RPATH for the build tree
  SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

  # when building, don't use the install RPATH already
  # (but later on when installing)
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

  if(APPLE)
    set(CMAKE_MACOSX_RPATH ON)
  endif(APPLE)

  SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

  # add the automatically determined parts of the RPATH
  # which point to directories outside the build tree to the install RPATH
  SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

  # the RPATH to be used when installing,
  # but only if it's not a system directory
  LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}" isSystemDir)
  IF("${isSystemDir}" STREQUAL "-1")
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
  ENDIF("${isSystemDir}" STREQUAL "-1")

ENDIF()

INCLUDE(${CMAKE_ROOT}/Modules/CheckLibraryExists.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckTypeSize.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckCXXSourceCompiles.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckCSourceCompiles.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckSymbolExists.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/GetPrerequisites.cmake)

INCLUDE(CheckCCompilerFlag)
INCLUDE(GenerateExportHeader)

###
# Various Options
##
# Default building shared libraries.
# BUILD_SHARED_LIBS is provided by/used by
# CMake directly.
OPTION(BUILD_SHARED_LIBS "Configure netCDF as a shared library." ON)
IF(BUILD_SHARED_LIBS)
  SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
ENDIF()


###
# Set up some DLL Options
###
IF(WIN32)
  OPTION(ENABLE_DLL "Build a Windows DLL." ${BUILD_SHARED_LIBS})
  IF(ENABLE_DLL)
    SET(BUILD_DLL ON CACHE BOOL "")
    ADD_DEFINITIONS(-DLL_EXPORT)
  ENDIF()
ENDIF()


# Set sources for developer library, build developer library.
SET(bzip2_lib_SOURCES blocksort.c
  huffman.c
  crctable.c
  randtable.c
  compress.c
  decompress.c
  bzlib.c)

ADD_LIBRARY(bzip2 ${bzip2_lib_SOURCES})

IF(MSVC)
  SET_TARGET_PROPERTIES(bzip2 PROPERTIES
  LINK_FLAGS_DEBUG " /NODEFAULTLIB:MSVCRT"
  )
ENDIF()

###
# End build library.
###

###
# Set up install directories.
###

IF(NOT DEFINED CMAKE_INSTALL_LIBDIR)
  SET(CMAKE_INSTALL_LIBDIR lib)
ENDIF()

IF(NOT DEFINED CMAKE_INSTALL_BINDIR)
  SET(CMAKE_INSTALL_BINDIR bin)
ENDIF()

IF(NOT DEFINED CMAKE_INSTALL_INCLUDEDIR)
  SET(CMAKE_INSTALL_INCLUDEDIR include)
ENDIF()

# Configure install locations, packages.
INSTALL(
  TARGETS bzip2
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT library
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

INSTALL(FILES ${CMAKE_SOURCE_DIR}/bzlib.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT headers)
