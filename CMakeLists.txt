cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()


project(voxel_engine)
project(voxel_engine VERSION 1.0)

add_subdirectory(dependencies)

add_executable(voxel_engine ./src/main.cpp)


target_link_libraries(voxel_engine PRIVATE glad)
target_link_libraries(voxel_engine PRIVATE glfw)
#target_link_libraries(voxel_engine PRIVATE glm)

target_include_directories(voxel_engine PUBLIC "${PROJECT_BINARY_DIR}") 