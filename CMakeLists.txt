cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()

project(voxel_engine)
project(voxel_engine VERSION 1.0)

option(USE_DEPENDENCIES "Configure with dependencies" ON)
# set(GLFW_USE_WAYLAND OFF CACHE BOOL "Disable Wayland for GLFW (linux)")
# set(GLFW_BUILD_X11 ON CACHE BOOL "Enable X11 backend in GLFW")

if(USE_DEPENDENCIES)
    add_subdirectory(dependencies)
endif()
add_subdirectory(engine)
add_subdirectory(game)
add_subdirectory(tests)

target_include_directories(Game SYSTEM PUBLIC "${PROJECT_SOURCE_DIR}/engine/src")
#target_include_directories(Game SYSTEM PUBLIC "${PROJECT_SOURCE_DIR}/")
# target_include_directories(Game PRIVATE dependencies/include)