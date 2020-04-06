cmake_minimum_required(VERSION 3.13)
project(InvPend)

option( INVPEND_ENABLE_SAMPLES "Compile samples if true." TRUE )
option( INVPEND_ENABLE_CLIENT "Compile client if true." TRUE )
option( INVPEND_ENABLE_SERVER "Compile server if true." TRUE )
option( INVPEND_ENABLE_ARDUINO "Compile arduino projects if true" TRUE )

set(INVPEND_INCLUDE_ONLY_DIR ${CMAKE_BINARY_DIR}/include)
set(INVPEND_COMMON_LIBS iplib)

message(STATUS "Setting up ${INVPEND_INCLUDE_ONLY_DIR} ...")
file(MAKE_DIRECTORY ${INVPEND_INCLUDE_ONLY_DIR}/iplib)
file(GLOB INVPEND_HEADER_FILES ${InvPend_SOURCE_DIR}/lib/*.h)
file(COPY ${INVPEND_HEADER_FILES} DESTINATION ${INVPEND_INCLUDE_ONLY_DIR}/iplib)
message(STATUS "Done setting up ${INVPEND_INCLUDE_ONLY_DIR} ...")

include(./CMakeIncludes/CMakeMacros.txt)

add_compile_options(-Wall -Wextra -Wpedantic)
add_compile_options(-g -O3 -std=c++17)

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