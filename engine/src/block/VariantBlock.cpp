#include "VariantBlock.h"

using namespace engine;

VariantBlock::VariantBlock(
    BlockID id, Layer layer, const Geometry* baseGeo, RotationMode rotationMode, int variantCount
)
    : Block(id, layer, baseGeo, rotationMode) {
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
        bool allConditionsMet = true;
        for (const auto& condition : variant.conditions) {
            if (!condition.blockIDs.contains(neighbours[condition.direction])) {
                allConditionsMet = false;
                break;
            }
        }
        if (allConditionsMet) {
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
        bool allConditionsMet = true;
        for (const auto& condition : variant.conditions) {
            if (!condition.blockIDs.contains(neighbours[condition.direction])) {
                allConditionsMet = false;
                break;
            }
        }
        if (allConditionsMet) {
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