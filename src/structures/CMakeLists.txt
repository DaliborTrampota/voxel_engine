

add_library(Structures
    Attributes.cpp Attributes.h
    Shader.cpp Shader.h
    ShaderPipeline.cpp ShaderPipeline.h
)

target_link_libraries(Structures PRIVATE glad)
target_link_libraries(Structures PRIVATE glm)
# target_link_libraries(Structures INTERFACE Game)
target_include_directories(Structures PUBLIC ${CMAKE_SOURCE_DIR}/src) #temp TODO