

set(LEVEL_SRC
    World.cpp World.h
    Chunk.cpp Chunk.h
    TerrainGenerator.cpp TerrainGenerator.h
)

list(TRANSFORM LEVEL_SRC PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
set(LEVEL_SRC ${LEVEL_SRC} PARENT_SCOPE)