#pragma once

#include <vector>

namespace gl {

    struct VertexAttribute {
        enum Type {
            Float,
            Int,
            UInt,
        };

        int location;
        Type type;
        size_t size;
        size_t offset;
    };

    /// Layout of attributes in shaders
    struct VertexLayout {
        /// Should be sizeof(VertexType)
        size_t stride;
        std::vector<VertexAttribute> attributes;
    };
}  // namespace gl