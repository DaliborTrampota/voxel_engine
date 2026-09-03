#pragma once

#include <block/Block.h>
#include <block/Geometry.h>

#include <initializer_list>
#include <vector>

namespace engine {

    struct Neighbours;

    /// @brief A variant block is a block that can have multiple geometries based on the surrounding blocks.
    /// @note The block is always using the base geometry if no variant is found.
    /// @note The block can be configured to always use the base geometry even if a variant is found.
    /// @note The block can be configured to allow multiple variants (thus geometries) to be used at the same time.
    /// @note If
    class VariantBlock : public Block {
      public:
        /// @brief Check if a rotation mode is valid for VariantBlock
        /// @note VariantBlock does not support AxisY and AxisXYZ rotation modes
        static constexpr bool isValidRotationMode(RotationMode mode) {
            return mode != RotationMode::AxisY && mode != RotationMode::AxisXYZ;
        }

        /// @brief A condition is a direction and a set of block IDs.
        /// @property direction The direction of the condition check
        /// @property blockIDs Possible block IDs for the direction. Only .
        /// @property requiredFacings Optional set of facing directions that the neighboring block must match.
        ///           If empty, any rotation is accepted. If set, the neighbor's facing must match one of these.
        /// @note The block IDs must be present in the direction for the variant to be used.
        /// @note If requiredFacings is set, the neighbor must have a BlockState with a matching facing direction.
        struct Condition {
            Side direction;
            std::vector<BlockID> blockIDs;  // unordered_set is more correct, but vector is faster
            std::vector<glm::vec3> requiredFacings;  // Optional: if empty, any rotation is valid

            bool operator==(const Condition& other) const {
                return direction == other.direction && blockIDs == other.blockIDs &&
                       requiredFacings == other.requiredFacings;
            }
        };

        /// @brief A variant is a specific geometry with a set of conditions.
        /// @property geometry The geometry of the variant.
        /// @property conditions The conditions of the variant.
        /// @note All conditions must be met for the variant to be used.
        struct Variant {
            Geometry geometry;
            std::vector<Condition> conditions;

            bool operator==(const Variant& other) const {
                return geometry.getID() == other.geometry.getID() && conditions == other.conditions;
            }
        };

        VariantBlock(
            BlockID id,
            Layer layer,
            const Geometry* baseGeo,
            RotationMode rotationMode,
            int variantCount
        );
        ~VariantBlock() override = default;


        VariantBlock& allowMultiple(bool allowMultiple);
        bool allowMultiple() const { return m_allowMultiple; }

        VariantBlock& alwaysUseBaseGeometry(bool alwaysUseBaseGeometry);
        bool alwaysUseBaseGeometry() const { return m_alwaysUseBaseGeometry; }

        VariantBlock& addVariant(
            const Geometry& geometry, std::initializer_list<Condition> conditions
        );

        VariantBlock& addVariant(Variant&& variant);

        /// @brief Get a variant that matches the neighbours.
        /// @param neighbours The neighbours of the block.
        /// @return The variant that matches the neighbours.
        /// @note Should be used if allowMultiple is false.
        const Variant* getVariant(const Neighbours& neighbours) const;

        /// @brief Get all variants that match the neighbours.
        /// @param neighbours The neighbours of the block.
        /// @return All variants that match the neighbours.
        /// @note Should be used if allowMultiple is true.
        std::vector<const Variant*> getVariants(const Neighbours& neighbours) const;

        /// @brief Get all geometries that match the neighbours and the base geometry if alwaysUseBaseGeometry is true.
        /// @param neighbours The neighbours of the block.
        /// @return All geometries that match the neighbours and the base geometry if alwaysUseBaseGeometry is true.
        std::vector<const Geometry*> getGeometries(const Neighbours& neighbours) const;

      private:
        // variant count is small, vector is faster than unordered_set
        std::vector<Variant> m_variants;
        bool m_allowMultiple = false;
        bool m_alwaysUseBaseGeometry = false;

        bool checkConditions(
            const std::vector<Condition>& conditions, const Neighbours& neighbours
        ) const;
    };
}  // namespace engine
