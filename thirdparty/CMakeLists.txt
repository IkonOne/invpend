cmake_minimum_required(VERSION 3.13)

add_subdirectory(box2d)

if (INVPEND_ENABLE_OPENGL)
    add_subdirectory(glad)
    add_subdirectory(glfw)
    add_subdirectory(imgui)
endif()