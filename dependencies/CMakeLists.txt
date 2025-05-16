cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(Dependencies)

add_library(tools INTERFACE)
add_library(glad STATIC glad.c)
add_subdirectory(glm)
add_subdirectory(glfw)

# set options before add_subdirectory
# available options : TRACY_ENABLE , TRACY_ON_DEMAND , TRACY_NO_BROADCAST , TRACY_NO_CODE_TRANSFER , ...
option(TRACY_ENABLE "" ON)
option(TRACY_ON_DEMAND "" ON)
add_subdirectory(tracy) # target : TracyClient or alias Tracy :: TracyClient

#add_subdirectory(tracy/profiler)

target_include_directories(tools INTERFACE include/)
target_include_directories(glad PUBLIC include/)

set_target_properties(glad PROPERTIES FOLDER "Libraries")
set_target_properties(glm PROPERTIES FOLDER "Libraries")
set_target_properties(TracyClient PROPERTIES FOLDER "Libraries")

set_target_properties(glfw PROPERTIES FOLDER "Libraries")
set_target_properties(uninstall PROPERTIES FOLDER "Libraries")
set_target_properties(update_mappings PROPERTIES FOLDER "Libraries")

#target_include_directories(glm PUBLIC glm/glm/)
