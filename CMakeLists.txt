cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE")

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()

project(voxel_engine)
project(voxel_engine VERSION 1.0)

add_subdirectory(dependencies)
add_subdirectory(engine)
add_subdirectory(game)
add_subdirectory(tests)

target_include_directories(Game SYSTEM PUBLIC "${PROJECT_SOURCE_DIR}/engine/src")
#target_include_directories(Game SYSTEM PUBLIC "${PROJECT_SOURCE_DIR}/")
# target_include_directories(Game PRIVATE dependencies/include)