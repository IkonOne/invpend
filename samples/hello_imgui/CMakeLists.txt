cmake_minimum_required(VERSION 3.13)

if (INVPEND_ENABLE_OPENGL)
    GETCURRENTFOLDER()
    STDPROJECT(${current_folder})
    target_link_libraries(${current_folder} glfw glad imgui)
endif()
