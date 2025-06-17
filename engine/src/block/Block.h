#pragma once

#include <string>

#include "Globals.h"
#include "Material.h"

namespace engine {
    class Geometry;

    class Block {
      public:
        Block(BlockID id, const Geometry* geo);
        Block(BlockID id, const Geometry* geo, const Material& mat);

        static Block& air();

        BlockID getID() const { return m_id; }

        const Geometry* geometry() const { return m_geometry; }

        bool isSolid() const { return m_isSolid; }
        bool isOpaque() const { return m_isOpaque; }
        bool isVoxel() const { return m_isVoxel; }


        Block& isOpaque(bool opaque) {
            m_isOpaque = opaque;
            return *this;
        }
        Block& isSolid(bool solid) {
            m_isSolid = solid;
            return *this;
        }
        Block& isVoxel(bool voxel) {
            m_isVoxel = voxel;
            return *this;
        }

        Block& material(const Material& mat) {
            m_material = mat;
            return *this;
        }
        Material& material() { return m_material; }

      private:
        BlockID m_id;
        bool m_isOpaque;
        bool m_isSolid;
        bool m_isVoxel;

        const Geometry* m_geometry;
        Material m_material;
    };
}  // namespace engine