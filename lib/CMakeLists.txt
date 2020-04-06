cmake_minimum_required(VERSION 3.13)
project(iplib)

file(GLOB INVPEND_HEADER_FILES ${InvPend_SOURCE_DIR}/lib/*.h)
file(GLOB INVPEND_SOURCE_FILES ${InvPend_SOURCE_DIR}/lib/*.cpp)

add_library(iplib STATIC ${INVPEND_SOURCE_FILES} ${INVPEND_HEADER_FILES})
