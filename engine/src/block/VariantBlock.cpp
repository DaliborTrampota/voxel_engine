#include "VariantBlock.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "utility/Algorithms.h"
#include "utility/CoordUtils.h"


using namespace engine;


BlockID VariantBlock::Neighbours::operator[](Side direction) const {
    if (direction == Side::North)
        return north;
    if (direction == Side::South)
        return south;
    if (direction == Side::East)
        return east;
    if (direction == Side::West)
        return west;
    if (direction == Side::Up)
        return up;
    if (direction == Side::Down)
        return down;
    return InvalidBlockID;  // Default/invalid direction
}

void VariantBlock::Neighbours::rotate(Side from, Side to) {
    if (from == to)
        return;

    Neighbours original = *this;

    glm::vec3 axis;
    float angle = -getAngleToSide(to, sideDirection(from), axis);

    glm::mat4 rotateMat = glm::rotate(glm::mat4(1.0f), angle, axis);
    auto rotateSide = [&](Side side) -> BlockID {
        glm::vec3 dir = sideDirection(side);
        // Use w=0 for direction vectors (not positions)
        glm::vec3 rotated = glm::vec3(rotateMat * glm::vec4(dir, 0.0f));

        // Round to nearest integer to handle floating-point precision errors
        // glm::ivec3 roundedDir = glm::round(rotated);

        return original[getSide(glm::round(rotated))];
        // Find the closest cardinal direction after rotation
        // Side closestSide = Side::North;
        // float maxDot = -2.0f;

        // for (Side testSide :
        //      {Side::North, Side::South, Side::East, Side::West, Side::Up, Side::Down}) {
        //     float dot = glm::dot(rotated, sideDirection(testSide));
        //     if (dot > maxDot) {
        //         maxDot = dot;
        //         closestSide = testSide;
        //     }
        // }

        // return original[closestSide];
    };

    north = rotateSide(Side::North);
    south = rotateSide(Side::South);
    east = rotateSide(Side::East);
    west = rotateSide(Side::West);
    up = rotateSide(Side::Up);
    down = rotateSide(Side::Down);
}

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