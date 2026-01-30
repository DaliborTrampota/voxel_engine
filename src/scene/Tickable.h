#pragma once

namespace engine {
    class Tickable {
      public:
        virtual void tick(float dt) = 0;
    };
}  // namespace engine