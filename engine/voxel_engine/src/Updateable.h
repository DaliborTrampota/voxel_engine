#pragma once

//#include "GameServices.h"

namespace engine {
    class Updateable {
      public:
        Updateable() = default;
        virtual ~Updateable() = default;

        virtual void update(float dt) = 0;
    };
}  // namespace engine