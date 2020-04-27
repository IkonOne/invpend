cmake_minimum_required(VERSION 3.13)

GETCURRENTFOLDER()
STDPROJECT(${current_folder})
target_link_libraries(${current_folder} box2d)