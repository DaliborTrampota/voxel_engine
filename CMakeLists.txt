cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()


project(voxel_engine)
project(voxel_engine VERSION 1.0)

add_subdirectory(dependencies)

source_group(TREE ./src)

add_executable(Game WIN32 src/main.cpp)

file(GLOB_RECURSE SHADER_FILES shaders/*.glsl)
# target_sources(Game PRIVATE ${SHADER_FILES})
add_custom_target(Shaders SOURCES ${SHADER_FILES})
source_group("Shaders" FILES ${SHADER_FILES})

target_link_libraries(Game PRIVATE glad)
target_link_libraries(Game PRIVATE glfw)
target_link_libraries(Game PRIVATE glm)
target_link_libraries(Game PRIVATE tools)

# target_include_directories(Game PRIVATE dependencies/include)
target_include_directories(Game PUBLIC "${PROJECT_BINARY_DIR}") 