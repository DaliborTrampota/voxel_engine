#include "VariantBlock.h"

#include <algorithm>
#include <stdexcept>

#include <glm/glm.hpp>

#include "block/Neighbours.h"
#include "utility/Algorithms.h"


using namespace engine;


VariantBlock::VariantBlock(
    BlockID id, Layer layer, const Geometry* baseGeo, RotationMode rotationMode, int variantCount
)
    : Block(id, layer, baseGeo, rotationMode) {
    if (!isValidRotationMode(rotationMode)) {
        throw std::invalid_argument(
            "VariantBlock does not support RotationMode::AxisY or RotationMode::AxisXYZ. "
            "Use Snap variants instead."
        );
    }
    m_variants.reserve(variantCount);
}

VariantBlock& VariantBlock::allowMultiple(bool allowMultiple) {
    m_allowMultiple = allowMultiple;
    return *this;
}

VariantBlock& VariantBlock::alwaysUseBaseGeometry(bool alwaysUseBaseGeometry) {
    m_alwaysUseBaseGeometry = alwaysUseBaseGeometry;
    return *this;
}

VariantBlock& VariantBlock::addVariant(
    const Geometry& geometry, std::initializer_list<Condition> conditions
) {
    Variant variant{geometry, std::vector<Condition>(conditions)};
    m_variants.push_back(std::move(variant));
    return *this;
}

VariantBlock& VariantBlock::addVariant(Variant&& variant) {
    m_variants.push_back(std::move(variant));
    return *this;
}

const VariantBlock::Variant* VariantBlock::getVariant(const Neighbours& neighbours) const {
    for (const auto& variant : m_variants) {
        if (checkConditions(variant.conditions, neighbours)) {
            return &variant;
        }
    }
    return nullptr;
}

// const Geometry& VariantBlock::getGeometry(const Neighbours& neighbours) const {
//     const Variant* variant = getVariant(neighbours);
//     if (variant) {
//         return variant->geometry;
//     }
//     return *m_geometry;
// }

std::vector<const VariantBlock::Variant*> VariantBlock::getVariants(
    const Neighbours& neighbours
) const {
    std::vector<const Variant*> variants;

    for (auto& variant : m_variants) {
        if (checkConditions(variant.conditions, neighbours)) {
            variants.push_back(&variant);
        }
    }
    return variants;
}

std::vector<const Geometry*> VariantBlock::getGeometries(const Neighbours& neighbours) const {
    std::vector<const Geometry*> geometries;
    if (m_alwaysUseBaseGeometry) {
        geometries.push_back(m_geometry);
    }

    if (m_allowMultiple) {
        for (const auto& variant : getVariants(neighbours)) {
            geometries.push_back(&variant->geometry);
        }
    } else {
        const auto* variant = getVariant(neighbours);
        if (variant) {
            geometries.push_back(&variant->geometry);
        }
    }
    return geometries;
}

bool VariantBlock::checkConditions(
    const std::vector<Condition>& conditions, const Neighbours& neighbours
) const {
    bool allConditionsMet = true;
    for (const auto& condition : conditions) {
        if (std::find(
                condition.blockIDs.begin(), condition.blockIDs.end(), neighbours[condition.direction]
            ) == condition.blockIDs.end()) {
            allConditionsMet = false;
            break;
        }

        if (!condition.requiredFacings.empty()) {
            glm::vec3 neighbourFacing = neighbours.getFacing(condition.direction);
            bool facingMatches = false;

            for (const auto& requiredFacing : condition.requiredFacings) {
                if (glm::length(neighbourFacing - requiredFacing) < 0.1f) {
                    facingMatches = true;
                    break;
                }
            }

            if (!facingMatches) {
                allConditionsMet = false;
                break;
            }
        }
    }
    return allConditionsMet;
}