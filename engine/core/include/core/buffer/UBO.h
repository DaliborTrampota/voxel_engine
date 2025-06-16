#pragma once

#include <string>
#include <vector>

namespace gl {

    enum class Type {
        Bool,
        Int,
        UInt,
        Float,
        Double,

        Vec2,
        Vec3,
        Vec4,
        IVec2,
        IVec3,
        IVec4,
        UVec2,
        UVec3,
        UVec4,
        BVec2,
        BVec3,
        BVec4,
        DVec2,
        DVec3,
        DVec4,

        Mat2,
        Mat3,
        Mat4,
        DMat2,
        DMat3,
        DMat4,
        //TODO Mat2x3 3x2 etc
    };

    /// @brief Get the alignment size for a given type.
    /// @note Alignment based on GLSL std140 layout rules: https://registry.khronos.org/OpenGL/extensions/ARB/ARB_uniform_buffer_object.txt
    constexpr size_t alignmentFor(Type type);
    constexpr size_t alignTo(size_t offset, size_t alignment);

    /// @brief Get the size in bytes for a given type.
    /// @note Alignment based on GLSL std140 layout rules: https://registry.khronos.org/OpenGL/extensions/ARB/ARB_uniform_buffer_object.txt
    constexpr size_t sizeFor(Type type);

    /// @brief Get the total size of a uniform buffer object layout. Order matters.
    /// @note Alignment based on GLSL std140 layout rules: https://registry.khronos.org/OpenGL/extensions/ARB/ARB_uniform_buffer_object.txt
    constexpr size_t layoutSize(const std::vector<Type>& layout);

    // glGetUniformBlockIndex glUniformBlockBinding no need to use, use binding = x in shader
    class UBO {
      public:
        UBO() = default;
        UBO(unsigned int bindingPoint, size_t size, const std::string& name);
        UBO(unsigned int bindingPoint, std::vector<Type> layout, const std::string& name);

        void create();
        void bindToProgram(unsigned int programID) const;

        void setSubData(size_t index, const void* data);
        void setData(const void* data) const;

      private:
        unsigned int m_id = 0;
        unsigned int m_bindingPoint;
        size_t m_size = 0;
        std::string m_blockName;
        std::vector<Type> m_layout;
    };
}  // namespace gl