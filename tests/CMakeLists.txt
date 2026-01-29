file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.h)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${SOURCES})

project(Test)

add_executable(Test WIN32 ${SOURCES})


target_link_libraries(Test PRIVATE glad)
target_link_libraries(Test PRIVATE glfw)

target_link_libraries(Test PUBLIC glm)
target_link_libraries(Test PUBLIC tools)
target_link_libraries(Test PUBLIC lwgl)

if(WIN32)
    target_link_options(Test PRIVATE "/SUBSYSTEM:CONSOLE")
endif()