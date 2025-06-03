#pragma once

#include <string>
#include <vector>

#include "Face.h"
#include "physics/AABB.h"
//#include "BlockTextures.h"


namespace engine {

    class Geometry {
      public:
        /// @brief Creates a geometry with given faces, and computes the AABB of them.
        /// @note It calculates the AABB using AABB::fromGeometry static method.
        Geometry(std::vector<Face> faces);

        /// @brief Creates a geometry with given faces and AABB.
        Geometry(std::vector<Face> faces, const AABB& aabb);

        unsigned int getID() const { return m_id; }
        const std::vector<Face>& faces() const { return m_faces; }
        const AABB& aabb() const { return m_aabb; }

        /// @brief Returns the geometry for basic cube. (1x1x1)
        static Geometry Cube();
        static Geometry Cylinder();

      private:
        static inline unsigned int s_idCounter = 0;
        Geometry();

        unsigned int m_id;
        std::vector<Face> m_faces;
        AABB m_aabb;
    };
}  // namespace engine