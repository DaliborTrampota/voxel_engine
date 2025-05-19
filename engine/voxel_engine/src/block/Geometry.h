#pragma once

#include <string>
#include <vector>

#include "Face.h"
#include "physics/AABB.h"
//#include "BlockTextures.h"


namespace engine {

    class Geometry {
      public:
        static inline unsigned int s_idCounter = 0;

        Geometry();
        Geometry(std::vector<Face> faces);

        unsigned int getID() const { return m_id; }
        const std::vector<Face>& faces() const { return m_faces; }

        static Geometry Cube();
        static Geometry Cylinder();

      private:
        unsigned int m_id;

        std::vector<Face> m_faces;
        AABB m_aabb;
    };
}  // namespace engine