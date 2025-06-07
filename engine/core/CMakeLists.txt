file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp src/*.h)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${SRC_FILES})
file(GLOB_RECURSE INC_FILES CONFIGURE_DEPENDS include/core/*.h)
# source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include/core" PREFIX "Include" FILES ${INC_FILES})

add_library(Core STATIC ${SRC_FILES})
target_sources(Core PRIVATE ${INC_FILES})

target_link_libraries(Core PRIVATE glfw)
target_link_libraries(Core PRIVATE glad)
target_link_libraries(Core PRIVATE glm)

target_include_directories(Core 
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include/core
        ${CMAKE_CURRENT_SOURCE_DIR}/include/core/texture
        ${CMAKE_CURRENT_SOURCE_DIR}/include/core/render
        ${CMAKE_CURRENT_SOURCE_DIR}/include/core/gl
        ${CMAKE_CURRENT_SOURCE_DIR}/include/core/events
)
# target_include_directories(Core SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)