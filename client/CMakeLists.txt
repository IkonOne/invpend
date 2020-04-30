cmake_minimum_required(VERSION 3.13)

if (INVPEND_ENABLE_OPENGL)
    MESSAGE_CONFIGURING()
    GETCURRENTFOLDER()
    STDPROJECT(${current_folder})

    add_subdirectory(client_sim)
endif()