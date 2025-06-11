#pragma once

#include <glad/glad.h>
#include <concepts>
#include <vector>

#include "../Globals.h"
#include "IBuffer.h"
#include "VertexLayout.h"


namespace gl {
    template <typename T>
    concept VertexType = requires(T t) {
        { T::layout() } -> std::same_as<VertexLayout>;
    };

    template <VertexType VertexT>
    class Attributes : public IBuffer {
      public:
        Attributes(GLint drawType = GL_STATIC_DRAW) : m_drawType(drawType), m_elements(0) {}
        virtual ~Attributes();

        void create();
        void reserve(int size) { m_data.reserve(size); }

        // IBuffer interface
        size_t length() const override { return m_elements; }
        size_t stride() const override { return VertexT::layout().stride; }
        const void* data() const override { return m_data.data(); }

        bool hasEBO() const override { return m_EBO != 0; }
        unsigned int elements() const override { return m_elements; }

        void clear() override {
            m_data.clear();
            m_data.reserve(4);
            m_dirty = true;
            m_elements = 0;
        }

        void bind() override {
            GL_GUARD
            glBindVertexArray(m_VAO);
            if (m_dirty) {
                glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
                glBufferData(
                    GL_ARRAY_BUFFER, m_data.size() * sizeof(VertexT), m_data.data(), m_drawType
                );
                m_dirty = false;
            }
        }
        // end of IBuffer interface

        std::vector<VertexT>& vertexData() { return m_data; }

        // TODO make more streamlined with Buffer?
        template <typename... Vals>
        inline void add(
            Vals&&... data
        ) {  //TODO better naming? and does this work how I think it does?
            m_data.emplace_back(data...);
            m_dirty = true;
            if (!hasEBO())
                m_elements++;
        }

        void move(VertexT&& vert) {
            m_data.push_back(std::move(vert));
            m_dirty = true;
            if (!hasEBO())
                m_elements++;
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

        std::vector<VertexT> m_data;
        bool m_dirty = true;
        int m_elements = 0;

        GLint m_drawType;
    };
}  // namespace gl


template <gl::VertexType VertexT>
gl::Attributes<VertexT>::~Attributes() {
    GL_GUARD
    if (m_VBO)
        glDeleteBuffers(1, &m_VBO);
    if (m_VAO)
        glDeleteVertexArrays(1, &m_VAO);
}

template <gl::VertexType VertexT>
void gl::Attributes<VertexT>::create() {
    GL_GUARD
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    VertexLayout layout = VertexT::layout();
    for (auto& attr : layout.attributes) {
        registerAttribute(attr.location, attr.size, attr.type, layout.stride, attr.offset);
    }
}