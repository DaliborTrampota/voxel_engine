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

source_group(TREE ./src)

add_executable(Game WIN32 src/main.cpp)



set(CORE
    src/main.cpp src/Globals.h 
    src/Camera.cpp src/Camera.h
    src/Game.cpp src/Game.h
    src/CoordUtils.cpp src/CoordUtils.h
)

set(PLAYER
    src/Player.cpp src/Player.h
)

add_subdirectory(src/structures)
add_subdirectory(src/block)
add_subdirectory(src/data)
add_subdirectory(src/level)

set(SRC_FILES
    ${CORE}
    ${PLAYER}
    ${BLOCK_SRC}
    src/TextureLoader.cpp src/TextureLoader.h
    ${DATA_SRC}
    ${LEVEL_SRC}
)

target_sources(Game PRIVATE
    src/block/Block.cpp
    src/Camera.cpp src/Camera.h
    src/TextureLoader.cpp
    ${SRC_FILES}
)

source_group(TREE ${CMAKE_SOURCE_DIR}/src PREFIX "Source Files" FILES
    src/main.cpp
    src/Camera.cpp
    src/Camera.h

    ${SRC_FILES}
    #src/block/Block.cpp
)

file(GLOB_RECURSE SHADER_FILES shaders/*.glsl)
# target_sources(Game PRIVATE ${SHADER_FILES})
add_custom_target(Shaders SOURCES ${SHADER_FILES})
source_group("Shaders" FILES ${SHADER_FILES})

target_link_libraries(Game PRIVATE glad)
target_link_libraries(Game PRIVATE glfw)
target_link_libraries(Game PRIVATE glm)
target_link_libraries(Game PRIVATE tools)
target_link_libraries(Game PRIVATE Structures)

# target_include_directories(Game PRIVATE dependencies/include)
target_include_directories(Game SYSTEM PUBLIC "${PROJECT_SOURCE_DIR}/src") 
#target_include_directories(Game SYSTEM PUBLIC "${PROJECT_SOURCE_DIR}/")
target_include_directories(Structures PUBLIC ${CMAKE_SOURCE_DIR}/src) # TEMP TODO


add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:Game>/resources
        ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:Game>/shaders
)
set_target_properties(Game PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Game>"
)