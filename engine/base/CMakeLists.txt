file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp src/*.h)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${SRC_FILES})

add_library(Base STATIC ${SRC_FILES})

target_link_libraries(Base PRIVATE glfw)
target_link_libraries(Base PRIVATE glad)
target_link_libraries(Base PRIVATE glm)