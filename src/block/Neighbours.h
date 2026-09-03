#pragma once

#include "../Globals.h"
#include "Side.h"


namespace engine {

    struct Neighbours {
        BlockID sides[6];
        glm::vec3 facings[6];

        BlockID operator[](Side direction) const;
        glm::vec3 getFacing(Side direction) const;
        void rotate(Side from, Side to);
    };

}  // namespace engine
