add_subdirectory(core)
add_subdirectory(voxel_engine)
set(ENGINE_RESOURCE_OUTPUT_DIR "${PROJECT_RUNTIME_OUTPUT_DIR}/e_resources")

target_include_directories(VoxelEngine SYSTEM PUBLIC "${PROJECT_SOURCE_DIR}/engine")
target_include_directories(VoxelEngine SYSTEM PUBLIC "${PROJECT_SOURCE_DIR}/engine/src")

# set_target_properties(VoxelEngine PROPERTIES FOLDER "Engine")
# set_target_properties(Graphics PROPERTIES FOLDER "Engine")