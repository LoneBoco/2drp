cmake_minimum_required(VERSION 3.27)

# Add custom CMake modules path for external dependencies.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build/cmake")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/dependencies")

# Includes.
include(FetchContent)
include(utility)

# Enable Hot Reload for MSVC compilers if supported.
if(POLICY CMP0141)
	cmake_policy(SET CMP0141 NEW)
	set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Policies.
set(CMAKE_POLICY_DEFAULT_CMP0054 NEW)	# CMP0054: Only interpret if() arguments as variables or keywords when unquoted.
set(CMAKE_POLICY_DEFAULT_CMP0074 NEW)	# CMP0074: find_package uses PackageName_ROOT variables.
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)	# CMP0077: option() honors normal variables.
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)	# CMP0091: MSVC runtime library flags are selected by an abstraction.

# vcpkg configuration.
set(VCPKG_TARGET_TRIPLET "x64-windows-static" CACHE STRING "vcpkg triplet")
set(VCPKG_TARGET_ARCHITECTURE "x64" CACHE STRING "vcpkg architecture")
set(VCPKG_INSTALLED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/vcpkg_installed")
set(VCPKG_CRT_LINKAGE STATIC)
set(VCPKG_LIBRARY_LINKAGE STATIC)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# 2drp project.
project(2drp
	VERSION 0.1
	DESCRIPTION "2drp"
	LANGUAGES CXX
)

# Get submodules.
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
	# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if (GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if (NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

# Set build type.
set(default_build_type "MinSizeRel")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
	set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
		STRING "Choose the type of build." FORCE)

	# Set the possible values of build type for cmake-gui
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
		"Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Debug build postfix.
set(CMAKE_DEBUG_POSTFIX _d)
set(BUILD_SHARED_LIBS OFF)

# Directories.
set(CMAKE_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/lib")

# Dependency and FetchContent setup.
set(DEPENDENCY_DIRECTORY "${PROJECT_SOURCE_DIR}/dependencies")
set(FETCHCONTENT_BASE_DIR "${DEPENDENCY_DIRECTORY}/fc")
set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE ALWAYS)

# System defines.
if(MSVC)
	add_compile_definitions(TDRP_WINDOWS UNICODE)
endif()

# Boost.
set(Boost_NO_WARN_NEW_VERSIONS ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost REQUIRED COMPONENTS filesystem)
include_directories("${Boost_INCLUDE_DIRS}")
link_directories("${Boost_LIBRARY_DIRS}")

# Lua.
find_package(Lua REQUIRED)
# find_package(PkgConfig REQUIRED)
# pkg_check_modules(LuaJIT REQUIRED IMPORTED_TARGET luajit)
set(SOL2_LUA_VERSION "5.4")
find_package(sol2 CONFIG REQUIRED)
set(LUA_INCLUDE_DIRECTORIES ${LUA_INCLUDE_DIR})
set(LUA_LINK_LIBRARIES ${LUA_LIBRARIES} sol2)
# set(LUA_LINK_LIBRARIES PkgConfig::LuaJIT sol2)

# Engine.
add_subdirectory(src/engine)

# Client.
add_subdirectory(src/client)

# Server.
