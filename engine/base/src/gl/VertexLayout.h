#pragma once

#include <vector>

namespace gl
{

    struct VertexAttribute
    {
        enum Type
        {
            Float,
            Int,
            UInt,
        };

        int location;
        Type type;
        size_t size;
        size_t offset;
        
    };

    struct VertexLayout
    {
        size_t stride;
        std::vector<VertexAttribute> attributes;
    };
}