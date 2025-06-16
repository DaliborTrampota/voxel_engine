#pragma once

#include <cstdint>
#include "VertexLayout.h"

namespace gl {
    class IBuffer {
      public:
        virtual ~IBuffer() = default;

        virtual void bind() = 0;
        virtual const void* data() const = 0;
        virtual size_t length() const = 0;
        virtual size_t stride() const = 0;
        virtual void clear() = 0;

        virtual bool hasEBO() const = 0;
        virtual unsigned int elements() const = 0;

      protected:
        void registerAttribute(
            uint32_t loc, uint32_t size, VertexAttribute::Type type, uint32_t stride, uint32_t offset
        );
    };
}  // namespace gl