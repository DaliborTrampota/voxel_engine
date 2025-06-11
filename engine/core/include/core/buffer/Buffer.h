#pragma once

#include "../Globals.h"
#include "IBuffer.h"
#include "VertexLayout.h"

#include <glad/glad.h>

namespace gl {

    enum class BufferType {
        Vertex,
        Index
    };

    /// @brief Simple buffer class for storing one shader layout data, eg. only position or only normals, stride is 0, offset is 0
    /// @tparam T type of the data stored in the buffer, must be simple numeric type (int, uint, float etc)
    template <typename T>
    class Buffer : public IBuffer {
      public:
        Buffer(GLint drawType = GL_STATIC_DRAW) : m_drawType(drawType) {}
        virtual ~Buffer() {
            GL_GUARD
            if (m_VBO) {
                glDeleteBuffers(1, &m_VBO);
            }
            if (m_VAO) {
                glDeleteVertexArrays(1, &m_VAO);
            }
        }

        // IBuffer interface
        size_t length() const override { return m_data.size() / m_attr.size; }
        size_t stride() const override { return 0; }
        const void* data() const override { return m_data.data(); }
        bool hasEBO() const override { return m_EBO != 0; }
        unsigned int elements() const override { return m_elements; }

        void clear() override {
            m_data.clear();
            m_data.reserve(4);
            m_elements = 0;
            m_dirty = true;
        }

        void bind() override {
            GL_GUARD

            glBindVertexArray(m_VAO);
            if (m_dirty) {
                glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
                glBufferData(GL_ARRAY_BUFFER, m_data.size() * sizeof(T), m_data.data(), m_drawType);
                m_dirty = false;
            }
        }

        // end of IBuffer interface

        void create(VertexAttribute attr) {
            GL_GUARD

            m_attr = std::move(attr);

            glGenVertexArrays(1, &m_VAO);
            glBindVertexArray(m_VAO);
            glGenBuffers(1, &m_VBO);
            glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

            registerAttribute(m_attr.location, m_attr.size, m_attr.type, 0, m_attr.offset);
        }

        void reserve(int size) { m_data.reserve(size); }

        template <typename... Vals>
        inline void add(Vals&&... data) {
            m_data.emplace_back(data...);
            m_dirty = true;
        }

        void add(T data) {
            m_data.push_back(data);
            m_dirty = true;
        }

        template <typename IndexT>
        void setElements(const std::vector<IndexT>& indices) {
            GL_GUARD

            glGenBuffers(1, &m_EBO);

            glBindVertexArray(m_VAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(IndexT), indices.data(), m_drawType
            );

            m_elements = indices.size();
        }

      private:
        unsigned int m_VAO = 0;
        unsigned int m_VBO = 0;
        unsigned int m_EBO = 0;

        VertexAttribute m_attr;

        std::vector<T> m_data;
        bool m_dirty = true;
        unsigned int m_elements = 0;

        GLint m_drawType;
    };
}  // namespace gl