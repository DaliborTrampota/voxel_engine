#pragma once

#include <glad/glad.h>
#include <vector>
#include <concepts>
#include <stdexcept>

#include "VertexLayout.h"
#include "../Globals.h"


namespace {
    constexpr GLsizei sizeOfType(GLenum type) {
        switch (type)
        {
            case GL_BYTE:
            case GL_UNSIGNED_BYTE:
                return sizeof(GLbyte);
            case GL_SHORT:
            case GL_UNSIGNED_SHORT:
                return sizeof(GLshort);
            case GL_INT_2_10_10_10_REV:
            case GL_INT:
            case GL_UNSIGNED_INT_2_10_10_10_REV:
            case GL_UNSIGNED_INT:
                return sizeof(GLint);
            case GL_FLOAT:
                return sizeof(GLfloat);
            case GL_DOUBLE:
                return sizeof(GLdouble);
            case GL_HALF_FLOAT:
                return sizeof(GLhalf);
            default: throw std::runtime_error("Unknown GL type");
        }
    }

    int getType(gl::VertexAttribute::Type type) {
        switch (type) {
            case gl::VertexAttribute::Float: return GL_FLOAT;
            case gl::VertexAttribute::Int: return GL_INT;
            case gl::VertexAttribute::UInt: return GL_UNSIGNED_INT;
            default: return GL_FLOAT;
        }
    }
}



namespace gl {
	template <typename T>
    concept VertexType = requires (T t) {
        { T::layout() } -> std::same_as<VertexLayout>;
    };

    template <VertexType VertexT>
    struct Attributes {
        Attributes() = default;
        Attributes(GLint drawType = GL_STATIC_DRAW) :
            m_drawType(drawType),
            m_elements(0)
        {}

        ~Attributes() {
            GL_GUARD
            if (m_VBO)
                glDeleteBuffers(1, &m_VBO);
            if (m_VAO)
                glDeleteVertexArrays(1, &m_VAO);
        }

        void create() {
            GL_GUARD
            glGenVertexArrays(1, &m_VAO);
            glBindVertexArray(m_VAO);
    
            glGenBuffers(1, &m_VBO);
            glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    
            VertexLayout layout = VertexT::layout();
            for (auto& attr : layout.attributes) {
                registerAttribute(attr.location, attr.size, getType(attr.type), layout.stride, attr.offset);
            }
        }
    
        void bind() {
            GL_GUARD
            glBindVertexArray(m_VAO);
            if(m_dirty) {
                glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
                glBufferData(GL_ARRAY_BUFFER, m_data.size() * sizeof(VertexT), m_data.data(), m_drawType);
                m_dirty = false;
            }
        }

    
        template <typename... Vals>
        inline void add(Vals&&... data) {
            m_data.emplace_back(data...);
            m_elements++;
            m_dirty = true;
        }
    
        void move(VertexT&& vert) {
            m_data.push_back(std::move(vert));
            m_elements++;
            m_dirty = true;
        }
    
        void reserve(int size) { m_data.reserve(size); }
        size_t length() const { return m_elements; }
    
        std::vector<VertexT> m_data;
    
    private:
        unsigned int m_VAO = 0;
        unsigned int m_VBO = 0;
        GLint m_drawType;
        int m_elements;
        bool m_dirty = true;

        
        /* @param stride If tightly packed then should be equal to size*/
        void registerAttribute(uint32_t loc, uint32_t size, GLenum type, uint32_t stride, uint32_t offset) const {
            GL_GUARD
            auto typeSize = sizeOfType(type);
            // always as floats in the shader (check glVertexAttribIPointer for ints and glVertexAttribLPointer for doubles)
            if (type == GL_FLOAT)
                glVertexAttribPointer(loc, size, type, GL_FALSE, stride, (void*)(uintptr_t(offset)));
            else if(type == GL_INT || type == GL_UNSIGNED_INT)
                glVertexAttribIPointer(loc, size, type, stride, (void*)(uintptr_t(offset)));
            glEnableVertexAttribArray(loc);
        }
    };
}