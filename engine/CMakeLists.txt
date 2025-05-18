add_subdirectory(core)
add_subdirectory(voxel_engine)



target_include_directories(VoxelEngine SYSTEM PUBLIC "${PROJECT_SOURCE_DIR}/engine")
target_include_directories(VoxelEngine SYSTEM PUBLIC "${PROJECT_SOURCE_DIR}/engine/src")

# set_target_properties(VoxelEngine PROPERTIES FOLDER "Engine")
# set_target_properties(Graphics PROPERTIES FOLDER "Engine")