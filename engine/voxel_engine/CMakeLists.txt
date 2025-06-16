file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp src/*.h)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${SRC_FILES})

add_library(VoxelEngine STATIC ${SRC_FILES})

target_link_libraries(VoxelEngine PRIVATE glad)
target_link_libraries(VoxelEngine PRIVATE glfw)

target_link_libraries(VoxelEngine PUBLIC glm)
target_link_libraries(VoxelEngine PUBLIC tools)
target_link_libraries(VoxelEngine PUBLIC LWGL)


target_include_directories(VoxelEngine SYSTEM PUBLIC "${PROJECT_SOURCE_DIR}/engine/LWGL/include")
target_include_directories(VoxelEngine SYSTEM PUBLIC "${PROJECT_SOURCE_DIR}/engine/voxel_engine/src")




file(GLOB_RECURSE ENGINE_RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/resources/*")

target_sources(VoxelEngine PRIVATE ${ENGINE_RESOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/resources" PREFIX "Resources" FILES ${ENGINE_RESOURCES})


function(setup_voxelengine_resource_copy TARGET_NAME)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/resources"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/resources"
        COMMENT "Copying VoxelEngine resources to build directory"
    )
endfunction()