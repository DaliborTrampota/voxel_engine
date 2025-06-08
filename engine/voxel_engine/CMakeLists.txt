file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp src/*.h)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${SRC_FILES})

add_library(VoxelEngine STATIC ${SRC_FILES})

target_link_libraries(VoxelEngine PRIVATE glad)
target_link_libraries(VoxelEngine PRIVATE glfw)

target_link_libraries(VoxelEngine PUBLIC glm)
target_link_libraries(VoxelEngine PUBLIC tools)
target_link_libraries(VoxelEngine PUBLIC Core)


target_include_directories(VoxelEngine SYSTEM PUBLIC "${PROJECT_SOURCE_DIR}/engine/core/include")
target_include_directories(VoxelEngine SYSTEM PUBLIC "${PROJECT_SOURCE_DIR}/engine/voxel_engine/src")

# set(ENGINE_RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources")

# # expose this to parent project
# set(ENGINE_RESOURCE_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/resources" PARENT_SCOPE)

# # Copy shaders to binary dir
# file(GLOB_RECURSE ENGINE_RESOURCES "${ENGINE_RESOURCE_DIR}/*")
# foreach(RESOURCE ${ENGINE_RESOURCES})
#     # Get the relative path of the shader file (preserve structure)
#     file(RELATIVE_PATH REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/resources" "${RESOURCE}")
#     set(DEST "${CMAKE_CURRENT_BINARY_DIR}/resources/${REL_PATH}")

#     configure_file(${RESOURCE} ${DEST} COPYONLY)
# endforeach()

# Allow parent to define this
if(NOT DEFINED PROJECT_RUNTIME_OUTPUT_DIR)
    message(STATUS "PROJECT_RUNTIME_OUTPUT_DIR is not defined. Please set it before including voxel_engine")
endif()

# Path to engine resources
set(ENGINE_RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources")
set(ENGINE_RESOURCE_OUTPUT_DIR "${PROJECT_RUNTIME_OUTPUT_DIR}/resources")
file(GLOB_RECURSE ENGINE_RESOURCES "${ENGINE_RESOURCE_DIR}/*")

add_custom_target(copy_engine_resources ALL)

foreach(RESOURCE ${ENGINE_RESOURCES})
    file(RELATIVE_PATH REL_PATH "${ENGINE_RESOURCE_DIR}" "${RESOURCE}")
    set(DEST "${ENGINE_RESOURCE_OUTPUT_DIR}/${REL_PATH}")

    # Make sure destination directory exists
    get_filename_component(DEST_DIR "${DEST}" DIRECTORY)

    add_custom_command(
        TARGET copy_engine_resources PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${DEST_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${RESOURCE}" "${DEST}"
        COMMENT "Copying resource: ${REL_PATH}"
        VERBATIM
    )
endforeach()