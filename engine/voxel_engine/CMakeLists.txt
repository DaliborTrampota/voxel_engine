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

# set_target_properties(VoxelEngine PROPERTIES FOLDER "Engine")
# set_target_properties(Graphics PROPERTIES FOLDER "Engine")