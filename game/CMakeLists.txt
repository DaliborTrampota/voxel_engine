file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.h)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${SOURCES})


add_executable(Game WIN32 ${SOURCES})
if(WIN32)
    target_link_options(Game PRIVATE "/SUBSYSTEM:CONSOLE")
endif()

target_link_libraries(Game PRIVATE VoxelEngine)

add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/game/resources $<TARGET_FILE_DIR:Game>/resources
)
add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/game/shaders $<TARGET_FILE_DIR:Game>/shaders
)
set_target_properties(Game PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Game>"
)




file(GLOB_RECURSE SHADER_FILES CONFIGURE_DEPENDS shaders/*.glsl)
file(GLOB_RECURSE RESOURCE_FILES CONFIGURE_DEPENDS resources/*)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/shaders" PREFIX "Shaders" FILES ${SHADER_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/resources" PREFIX "Resources" FILES ${RESOURCE_FILES})

target_sources(Game PRIVATE ${RESOURCE_FILES})

# Shaders
# file(GLOB_RECURSE SHADER_FILES shaders/*.glsl)
# add_custom_target(Shaders SOURCES ${SHADER_FILES})
# source_group("Shaders" FILES ${SHADER_FILES})
