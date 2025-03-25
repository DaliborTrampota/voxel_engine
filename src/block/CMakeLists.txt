

set(BLOCK_SRC
    Block.cpp Block.h
    BlockData.cpp BlockData.h
    builder/Geometry.cpp builder/Geometry.h
    builder/Face.cpp builder/Face.h
    #builder/CubeGeometry.cpp builder/CubeGeometry.h
    #builder/CulledGeometry.cpp builder/CulledGeometry.h
)

list(TRANSFORM BLOCK_SRC PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
set(BLOCK_SRC ${BLOCK_SRC} PARENT_SCOPE)