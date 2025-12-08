#pragma once

#include "BlockMaterial.h"
#include "Globals.h"

// #include "render/BlockMaterial.h"

namespace engine {
    class Geometry;

    enum class Side {
        North,
        South,
        East,
        West,
        Up,
        Down,
    };
    enum class RotationMode {
        None,
        AxisY,
        AnyAxis,
        AnyAxisAngle,
    };

    class Block {
      public:
        Block(BlockID id, Layer layer, const Geometry* geo);
        Block(BlockID id, Layer layer, const Geometry* geo, RotationMode rotationMode);
        virtual ~Block() = default;

        static Block& air();
        bool isAir() const { return m_id == 0; }

        BlockID getID() const { return m_id; }

        const Geometry* geometry() const { return m_geometry; }

        bool isSolid() const { return m_isSolid; }
        bool isVoxel() const { return m_isVoxel; }
        Layer layer() const { return m_layer; }
        RotationMode rotationMode() const { return m_rotationMode; }

        Block& rotationMode(RotationMode mode);
        Block& isSolid(bool solid);
        Block& isVoxel(bool voxel);

        Block& material(const BlockMaterial& mat);
        const BlockMaterial& material() const;

      protected:
        bool m_isSolid;
        bool m_isVoxel;
        Layer m_layer;
        RotationMode m_rotationMode = RotationMode::None;

        const Geometry* m_geometry;
        BlockMaterial m_material;

      private:
        BlockID m_id;
    };
}  // namespace engine