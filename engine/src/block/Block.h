#pragma once

#include "BlockMaterial.h"
#include "Globals.h"
#include "Side.h"


// #include "render/BlockMaterial.h"

namespace engine {
    class Geometry;

    /// @brief The rotation mode of the block.
    /// @note None is for non rotational blocks, eg stone, dirt, sand
    /// @note AxisYSnap and AxisXYZSnap are used to snap the block to the axis when placed. Meaning the block will be always aligned with some axis. Y axis is used for blocks that can be rotated but can't face up. Blocks with XYZ axis can face up.
    /// @note AxisY and AxisXYZ is used for special needs. Block can be rotated by any angle and doesn't have to be aligned with axis.
    /// @note AxisAngleXYZSnap for blocks that can face any axis aligned direction and in addition be rotated around Y axis when pointing up or down.
    /// @note AxisAlign is used for column blocks, where positive and negative axis direction does not matter (eg (1, 0, 0) and (-1, 0, 0) looks the same)
    enum class RotationMode {
        None,
        AxisY,
        AxisYSnap,
        AxisXYZ,
        AxisXYZSnap,
        AxisAngleXYZSnap,
        AxisAlign,
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
        bool facingUp() const { return m_faceUp; }
        Layer layer() const { return m_layer; }
        RotationMode rotationMode() const { return m_rotationMode; }

        Block& rotationMode(RotationMode mode);
        Block& isSolid(bool solid);
        Block& isVoxel(bool voxel);
        /// @note Denotes the base orientation of the block model and texture. Blocks have to be modeled so the base orientation is either up or north.
        Block& facingUp(bool state);

        Block& material(const BlockMaterial& mat);
        const BlockMaterial& material() const;

      protected:
        bool m_isSolid;
        bool m_isVoxel;
        bool m_faceUp = false;
        Layer m_layer;
        RotationMode m_rotationMode = RotationMode::None;

        const Geometry* m_geometry;
        BlockMaterial m_material;

      private:
        BlockID m_id;
    };
}  // namespace engine