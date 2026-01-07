#pragma once

#include <iosfwd>

namespace engine {
    class ISerializable {
      public:
        virtual ~ISerializable() = default;

        virtual void serialize(std::ostream& out) const = 0;
        virtual void deserialize(std::istream& in) = 0;
    };
}  // namespace engine