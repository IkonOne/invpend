cmake_minimum_required(VERSION 3.13)
project(iplib)

file(GLOB INVPEND_DESKTOP_HEADER_FILES ${InvPend_SOURCE_DIR}/lib/*.h ${InvPend_SOURCE_DIR}/lib/desktop/*.h)
file(GLOB INVPEND_DESKTOP_SOURCE_FILES ${InvPend_SOURCE_DIR}/lib/*.cpp ${InvPend_SOURCE_DIR}/lib/desktop/*.cpp)
file(GLOB INVPEND_ARDUINO_HEADER_FILES ${InvPend_SOURCE_DIR}/lib/*.h ${InvPend_SOURCE_DIR}/lib/arduino/*.h)
file(GLOB INVPEND_ARDUINO_SOURCE_FILES ${InvPend_SOURCE_DIR}/lib/*.cpp ${InvPend_SOURCE_DIR}/lib/arduino/*.cpp)

add_library(iplib STATIC ${INVPEND_DESKTOP_SOURCE_FILES})
target_include_directories(iplib PUBLIC ${InvPend_SOURCE_DIR}/lib ${InvPend_SOURCE_DIR}/lib/desktop)
