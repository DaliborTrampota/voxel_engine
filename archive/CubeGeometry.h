#pragma once

#include <glm/glm.hpp>

#include <array>
#include <vector>

#include "Block.h"
#include "Face.h"
#include "Geometry.h"


namespace engine {

    class CubeGeometry : public Geometry {
      public:
        CubeGeometry(TexID texture);
        CubeGeometry(TexID textures[6]);

        bool sideSpecific();  // TODO? override;

        Face getFace(int side) const;
        TexID getTexture(int side) const;

        void build(float scale, glm::vec3 offset, bool scaleUVs = false);

      private:
        std::array<Face, 6> m_faces;
        TexID m_textures[6];
    };
}  // namespace engine