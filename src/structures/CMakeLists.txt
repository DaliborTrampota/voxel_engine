

add_library(Structures STATIC
    Globals.h
    GraphicsAPI.cpp GraphicsAPI.h CallbackWrapper.h

    events/Event.h events/EventSite.h
    events/EventSource.h events/EventSource.cpp

    GLEventSite.h GLEvents.h
    GLEventSource.h GLEventSource.cpp

    gl/Attributes.cpp gl/Attributes.h
    gl/Shader.cpp gl/Shader.h
    gl/ShaderPipeline.cpp gl/ShaderPipeline.h
    ThreadPool.h
)

target_link_libraries(Structures PRIVATE glfw)
target_link_libraries(Structures PRIVATE glad)
target_link_libraries(Structures PRIVATE glm)
# target_link_libraries(Structures INTERFACE Game)
target_include_directories(Structures PUBLIC ${CMAKE_SOURCE_DIR}/src) #temp TODO