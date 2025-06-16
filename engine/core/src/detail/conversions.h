#pragma once

#include <glad/glad.h>
#include <stdexcept>

#include "buffer/VertexLayout.h"

namespace gl::detail {

    constexpr GLsizei sizeOfType(GLenum type) {
        switch (type) {
            case GL_BYTE:
            case GL_UNSIGNED_BYTE: return sizeof(GLbyte);
            case GL_SHORT:
            case GL_UNSIGNED_SHORT: return sizeof(GLshort);
            case GL_INT_2_10_10_10_REV:
            case GL_INT:
            case GL_UNSIGNED_INT_2_10_10_10_REV:
            case GL_UNSIGNED_INT: return sizeof(GLint);
            case GL_FLOAT: return sizeof(GLfloat);
            case GL_DOUBLE: return sizeof(GLdouble);
            case GL_HALF_FLOAT: return sizeof(GLhalf);
            default: throw std::runtime_error("Unknown GL type");
        }
    }

    constexpr int getType(gl::VertexAttribute::Type type) {
        switch (type) {
            case gl::VertexAttribute::Float: return GL_FLOAT;
            case gl::VertexAttribute::Int: return GL_INT;
            case gl::VertexAttribute::UInt: return GL_UNSIGNED_INT;
            // case gl::VertexAttribute::Byte: return GL_BYTE;
            // case gl::VertexAttribute::UByte: return GL_UNSIGNED_BYTE;
            // case gl::VertexAttribute::Short: return GL_SHORT;
            // case gl::VertexAttribute::UShort: return GL_UNSIGNED_SHORT;
            // case gl::VertexAttribute::Double: return GL_DOUBLE;
            default: return GL_FLOAT;
        }
    }

}  // namespace gl::detail