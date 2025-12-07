#pragma once

#include "BlockMaterial.h"
#include "Globals.h"

// #include "render/BlockMaterial.h"

namespace engine {
    class Geometry;

    class Block {
      public:
        Block(BlockID id, Layer layer, const Geometry* geo);
        Block(BlockID id, Layer layer, const Geometry* geo, const BlockMaterial& mat);

        static Block& air();
        bool isAir() const { return m_id == 0; }

        BlockID getID() const { return m_id; }

        const Geometry* geometry() const { return m_geometry; }

        bool isSolid() const { return m_isSolid; }
        bool isVoxel() const { return m_isVoxel; }
        Layer layer() const { return m_layer; }


        Block& isSolid(bool solid) {
            m_isSolid = solid;
            return *this;
        }
        Block& isVoxel(bool voxel) {
            m_isVoxel = voxel;
            return *this;
        }

        Block& material(const BlockMaterial& mat) {
            m_material = mat;
            return *this;
        }
        BlockMaterial& material() { return m_material; }

      private:
        BlockID m_id;
        bool m_isSolid;
        bool m_isVoxel;
        Layer m_layer;

        const Geometry* m_geometry;
        BlockMaterial m_material;
    };
}  // namespace engine