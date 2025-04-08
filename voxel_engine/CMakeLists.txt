cmake_minimum_required(VERSION 3.29)
project(VoxelEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


macro(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        SET(dirlist ${dirlist} ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
endmacro()




include(FetchContent)

FetchContent_Declare(
    imgui-glfw-glad-glm
    GIT_REPOSITORY https://github.com/cmmw/imgui-glfw-glad-glm.git
    GIT_TAG v4.2.0
)

FetchContent_MakeAvailable(imgui-glfw-glad-glm)

# file(GLOB_RECURSE sources CONFIGURE_DEPENDS RELATIVE ${CMAKE_SOURCE_DIR}
#      "src/*.h"
#      "src/*.cpp"
# )


# I used this for allowing CMake to include all the files,
# also new ones, without having to touch the CMakeLists.txt
file(GLOB_RECURSE PROJECT_SOURCE_FILES "*.h" "*.hpp" "*.cpp")
file(GLOB_RECURSE SHADERS "*.glsl")
 
# I then wrote this Macro, which organize the files in visual studio folders,
# according to the organization in the file systems
 
macro(GroupSources curdir)
   file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir}
${PROJECT_SOURCE_DIR}/${curdir}/*)
   foreach(child ${children})
          if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
                  GroupSources(${curdir}/${child})
          else()
                  string(REPLACE "/" "\\" groupname ${curdir})
          # I would like to call the src root folder in a different name, only in visual studio (not mandatory requirement)
                   string(REPLACE "src" "Common" groupname ${groupname})
            source_group(${groupname} FILES
${PROJECT_SOURCE_DIR}/${curdir}/${child})
          endif()
   endforeach()
endmacro()
   
# Execute the macro
GroupSources(src)
GroupSources(libs)

 
add_library(${PROJECT_NAME} ${PROJECT_SOURCE_FILES} ${SHADERS})



# add_executable(VoxelEngine ${sources})

# It is sufficient to only link glm and ImGui since the latter one already contains Glad and GLFW
target_link_libraries(
    VoxelEngine
    imgui
    glm
)


