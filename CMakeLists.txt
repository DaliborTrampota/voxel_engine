cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()

project(voxel_engine VERSION 1.0)

option(USE_DEPENDENCIES "Configure with dependencies" ON)
option(BUILD_VOXEL_ENGINE_TESTS "Build the Voxel Engine tests" OFF)
# set(GLFW_USE_WAYLAND OFF CACHE BOOL "Disable Wayland for GLFW (linux)")
# set(GLFW_BUILD_X11 ON CACHE BOOL "Enable X11 backend in GLFW")



file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp src/*.h)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${SRC_FILES})

add_library(VoxelEngine STATIC ${SRC_FILES})


file(GLOB_RECURSE ENGINE_RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/resources/*")
target_sources(VoxelEngine PRIVATE ${ENGINE_RESOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/resources" PREFIX "Resources" FILES ${ENGINE_RESOURCES})


if(USE_DEPENDENCIES)
    add_subdirectory(dependencies)
endif()

target_link_libraries(VoxelEngine PRIVATE glad)
target_link_libraries(VoxelEngine PRIVATE glfw)

target_link_libraries(VoxelEngine PUBLIC glm)
target_link_libraries(VoxelEngine PUBLIC tools)
target_link_libraries(VoxelEngine PUBLIC lwgl)
target_link_libraries(VoxelEngine PUBLIC soloud)


target_include_directories(VoxelEngine SYSTEM PUBLIC "${PROJECT_SOURCE_DIR}/src")


function(setup_voxelengine_resource_copy TARGET_NAME)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/resources"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/resources"
        COMMENT "Copying VoxelEngine resources to build directory"
    )
endfunction()

if(BUILD_VOXEL_ENGINE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()


# Treat warnings as errors
# target_compile_options(VoxelEngine PRIVATE
#     $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
#     $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Werror>
# )
