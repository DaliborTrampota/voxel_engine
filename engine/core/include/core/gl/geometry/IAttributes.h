#pragma once


namespace gl {
    struct IAttributes {
        virtual ~IAttributes() = default;

        virtual void bind() = 0;
        virtual const void* data() const = 0;
        virtual size_t length() const = 0;
        virtual size_t stride() const = 0;
    };
}  // namespace gl