#include "UBO.h"
#include "Globals.h"

#include <glad/glad.h>

using namespace gl;


constexpr size_t gl::alignmentFor(Type type) {
    switch (type) {
        case Type::Bool:
        case Type::Int:
        case Type::UInt:
        case Type::Float: return 4;

        case Type::Double: return 8;

        case Type::Vec2:
        case Type::IVec2:
        case Type::UVec2:
        case Type::BVec2: return 8;

        case Type::Vec3:
        case Type::Vec4:
        case Type::IVec3:
        case Type::IVec4:
        case Type::UVec3:
        case Type::UVec4:
        case Type::BVec3:
        case Type::BVec4: return 16;

        case Type::DVec2: return 16;  // 2×8 = 16

        case Type::DVec3:
        case Type::DVec4: return 32;  // 4×8 (rounded up)

        case Type::Mat2: return 16;  // 2 columns of vec2, each aligned to vec4

        case Type::Mat3:
        case Type::Mat4: return 16;  // column-major: 3 or 4 vec4-aligned columns

        case Type::DMat2: return 32;  // 2 columns of dvec2 (each 16-aligned, padded to 32)

        case Type::DMat3:
        case Type::DMat4: return 32;  // column-major dvec3/dvec4 (aligned to dvec4, 32 bytes)
    }
    return 0;
}

constexpr size_t gl::alignTo(size_t offset, size_t alignment) {
    return (offset + alignment - 1) &
           ~(alignment - 1);  // same as ((offset + alignment - 1) / alignment) * alignment
}

constexpr size_t gl::sizeFor(Type type) {
    switch (type) {
        case Type::Bool:
        case Type::Int:
        case Type::UInt:
        case Type::Float: return 4;

        case Type::Double: return 8;

        case Type::Vec2:
        case Type::IVec2:
        case Type::UVec2:
        case Type::BVec2: return 8;

        case Type::Vec3: return 12;
        case Type::IVec3:
        case Type::UVec3:
        case Type::BVec3: return 12;

        case Type::Vec4:
        case Type::IVec4:
        case Type::UVec4:
        case Type::BVec4: return 16;

        case Type::DVec2: return 16;

        case Type::DVec3: return 24;
        case Type::DVec4: return 32;

        // Column-major matrices
        case Type::Mat2: return 2 * 16;  // 2 aligned columns (vec2 padded to vec4)

        case Type::Mat3: return 3 * 16;  // 3 columns of vec3 → padded to 3×vec4

        case Type::Mat4: return 4 * 16;

        case Type::DMat2: return 2 * 32;  // 2 columns of dvec2 aligned to 32

        case Type::DMat3: return 3 * 32;  // 3 columns of dvec3 padded to dvec4

        case Type::DMat4: return 4 * 32;
    }
    return 0;
}

constexpr size_t s_vec4Alignment = 16;
constexpr size_t gl::layoutSize(const std::vector<Type>& layout) {
    size_t offset = 0;
    for (auto type : layout) {
        size_t alignment = alignmentFor(type);
        offset = alignTo(offset, alignment);
        size_t size = sizeFor(type);
        offset += size;

        // Rule (3): vec3 must be padded to vec4
        if (type == Type::Vec3) {
            offset = alignTo(offset, s_vec4Alignment);
        }
    }
    return alignTo(offset, s_vec4Alignment);  // final padding to vec4 alignment
}

UBO::UBO(unsigned int bindingPoint, size_t size, const std::string& name)
    : m_size(size),
      m_bindingPoint(bindingPoint),
      m_blockName(name) {}

UBO::UBO(unsigned int bindingPoint, std::vector<Type> layout, const std::string& name)
    : m_size(layoutSize(layout)),
      m_layout(layout),
      m_bindingPoint(bindingPoint),
      m_blockName(name) {}

void UBO::create() {
    GL_GUARD
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_UNIFORM_BUFFER, m_id);
    glBufferData(GL_UNIFORM_BUFFER, m_size, nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);  // unbind

    glBindBufferRange(GL_UNIFORM_BUFFER, m_bindingPoint, m_id, 0, m_size);
    printf("UBO %s created with ID %d, size %zu bytes\n", m_blockName.c_str(), m_id, m_size);
}

void UBO::bindToProgram(unsigned int programID) const {
    GL_GUARD
    unsigned int blockIndex = glGetUniformBlockIndex(programID, m_blockName.c_str());
    if (blockIndex == GL_INVALID_INDEX) {
        printf(
            "UBO::bindToProgram: uniform block \"%s\" not found or not active!\n",
            m_blockName.c_str()
        );
        return;
    }
    glUniformBlockBinding(programID, blockIndex, m_bindingPoint);
}

void UBO::setSubData(size_t index, const void* data) {
    GL_GUARD
    if (index >= m_layout.size()) {
        printf("UBO::setSubData: index out of bounds\n");
        return;
    }
    glBindBuffer(GL_UNIFORM_BUFFER, m_id);


    size_t offset = layoutSize(std::vector<Type>(m_layout.begin(), m_layout.begin() + index));
    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeFor(m_layout[index]), data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


void UBO::setData(const void* data) const {
    GL_GUARD
    glBindBuffer(GL_UNIFORM_BUFFER, m_id);
    glBufferData(GL_UNIFORM_BUFFER, m_size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}