#include "buffer/IBuffer.h"

#include <glad/glad.h>
#include "../detail/conversions.h"
#include "Globals.h"

void gl::IBuffer::registerAttribute(
    uint32_t loc, uint32_t size, VertexAttribute::Type type, uint32_t stride, uint32_t offset
) {
    GL_GUARD
    GLenum glType = detail::getType(type);
    if (glType == GL_FLOAT)
        glVertexAttribPointer(loc, size, glType, GL_FALSE, stride, (void*)(uintptr_t(offset)));
    else if (glType == GL_INT || glType == GL_UNSIGNED_INT)
        glVertexAttribIPointer(loc, size, glType, stride, (void*)(uintptr_t(offset)));
    glEnableVertexAttribArray(loc);
}