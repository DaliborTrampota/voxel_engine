#pragma once

#include <vector>

namespace gl {

    struct VertexAttribute {
        enum Type {
            Float,
            Int,
            UInt,
        };

        /// @brief Location in the shader, must be unique
        int location;

        /// @brief Type of the attribute
        Type type;

        /// @brief Number of components, eg. 3 for vec3, 4 for vec4
        size_t size;

        /// @brief Offset in bytes from the start of the vertex data, used for interleaved attributes
        /// @note If tightly packed then should be 0
        size_t offset;
    };

    /// @brief Layout of attributes in shaders
    struct VertexLayout {
        /// @note Should be sizeof(VertexType)
        size_t stride;
        std::vector<VertexAttribute> attributes;
    };
}  // namespace gl