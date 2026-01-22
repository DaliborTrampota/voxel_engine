add_library(tools INTERFACE)
add_library(glad STATIC glad.c)
add_subdirectory(glm)

# Disable GLFW extras we don't need (can be overridden via cmake -DGLFW_BUILD_DOCS=ON, etc.)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)

add_subdirectory(glfw)
add_subdirectory(lwgl)
add_subdirectory(SoLoud)

target_include_directories(tools INTERFACE include/)
target_include_directories(glad PUBLIC include/)

set_target_properties(glad PROPERTIES FOLDER "Libraries")
set_target_properties(glm PROPERTIES FOLDER "Libraries")
set_target_properties(lwgl PROPERTIES FOLDER "Libraries")
set_target_properties(soloud PROPERTIES FOLDER "Libraries")

set_target_properties(glfw PROPERTIES FOLDER "Libraries/GLFW3")

# Set properties for optional GLFW targets only if they exist
if(TARGET update_mappings)
    set_target_properties(update_mappings PROPERTIES FOLDER "Libraries/GLFW3")
endif()

#target_include_directories(glm PUBLIC glm/glm/)