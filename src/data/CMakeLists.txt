

set(DATA_SRC
    registry/Registry.cpp registry/Registry.h

    TextureManager.cpp TextureManager.h
    Vertex.cpp Vertex.h
    VertexData.h
)

list(TRANSFORM DATA_SRC PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
set(DATA_SRC ${DATA_SRC} PARENT_SCOPE)
#target_include_directories(Game SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})