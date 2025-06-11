#pragma once

#include "IAttributes.h"
#include "VertexLayout.h"

namespace gl {


    template <typename T>
    struct Buffer : public IAttributes {
        Buffer(GLint drawType = GL_STATIC_DRAW) : m_drawType(drawType) {}

        void create(VertexAttribute attr) {
            GL_GUARD

            m_attr = std::move(attr);

            glGenBuffers(1, &m_VBO);
            glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

            registerAttribute(m_attr.location, m_attr.size, getType(m_attr.type), 0, m_attr.offset);
        }

        void bind() override { glBindBuffer(GL_ARRAY_BUFFER, m_VBO); }
        size_t length() const override { return m_data.size() / m_attr.size; }
        size_t stride() const override { return 0; }
        const void* data() const override { return m_data.data(); }

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

      private:
        unsigned int m_VBO = 0;
        std::vector<T> m_data;
        int m_stride = 0;  // stride in elements
        VertexAttribute m_attr;
        GLint m_drawType;
        bool m_dirty = true;
    };

    template <typename T>
    void Buffer<T>::bind() {
        GL_GUARD

        if (m_dirty) {
            glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
            glBufferData(GL_ARRAY_BUFFER, m_data.size() * sizeof(T), m_data.data(), m_drawType);
            m_dirty = false;
        }
    }
}  // namespace gl