cmake_minimum_required(VERSION 3.13)
project(InvPend)

option( INVPEND_ENABLE_SAMPLES "Compile samples if on" OFF)
option( INVPEND_ENABLE_CLIENT "Compile client if on" OFF)
option( INVPEND_ENABLE_SERVER "Compile server if on" ON)
option( INVPEND_ENABLE_ARDUINO "Compile arduino projects if on" OFF)
option( INVPEND_ENABLE_TOOLS "Compile debug tooling if on" ON)
option( INVPEND_ENABLE_OPENGL "Builds iplib opengl" OFF)

set(INVPEND_INCLUDE_ONLY_DIR ${CMAKE_BINARY_DIR}/include)
set(INVPEND_COMMON_LIBS iplib)

include(./CMakeIncludes/CMakeMacros.txt)

add_subdirectory(thirdparty)
add_subdirectory(lib)

if( INVPEND_ENABLE_SAMPLES )
	add_subdirectory(samples)
endif()

if( INVPEND_ENABLE_CLIENT )
	add_subdirectory(client)
endif()

if( INVPEND_ENABLE_SERVER )
	add_subdirectory(server)
endif()

if( INVPEND_ENABLE_ARDUINO )
	add_subdirectory(arduino)
endif()

if( INVPEND_ENABLE_TOOLS )
	add_subdirectory(tools)
endif()