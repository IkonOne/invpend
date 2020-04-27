cmake_minimum_required(VERSION 3.13)

find_package(OpenGL REQUIRED)

file(GLOB IMGUI_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set(IMGUI_LIBRARIES OpenGL::GL glad glfw )

add_compile_options(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)
add_library(imgui STATIC ${IMGUI_SOURCE_FILES})
target_link_libraries(imgui ${IMGUI_LIBRARIES})

target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

list(APPEND IMGUI_LIBRARIES imgui)