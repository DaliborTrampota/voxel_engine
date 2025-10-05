cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(Dependencies)

add_library(tools INTERFACE)
add_library(glad STATIC glad.c)
add_subdirectory(glm)
add_subdirectory(glfw)
add_subdirectory(lwgl)
add_subdirectory(seuil)

target_include_directories(tools INTERFACE include/)
target_include_directories(glad PUBLIC include/)

set_target_properties(glad PROPERTIES FOLDER "Libraries")
set_target_properties(glm PROPERTIES FOLDER "Libraries")

set_target_properties(glfw PROPERTIES FOLDER "Libraries")
set_target_properties(uninstall PROPERTIES FOLDER "Libraries")
set_target_properties(update_mappings PROPERTIES FOLDER "Libraries")

#target_include_directories(glm PUBLIC glm/glm/)