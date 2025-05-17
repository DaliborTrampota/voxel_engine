file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.h)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${SOURCES})


add_executable(Test WIN32 ${SOURCES})


if(WIN32)
    target_link_options(Test PRIVATE "/SUBSYSTEM:CONSOLE")
endif()