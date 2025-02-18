cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(Dependencies)

add_library(glad STATIC glad.c)
add_subdirectory(include/glm)

target_include_directories(glad PUBLIC include/)
target_include_directories(glm INTERFACE include/)