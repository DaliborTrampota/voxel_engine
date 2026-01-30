#include "VariantBlock.h"

#include <algorithm>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "utility/Algorithms.h"
#include "utility/Rotation.h"


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

glm::vec3 VariantBlock::Neighbours::getFacing(Side direction) const {
    if (direction == Side::North)
        return northFacing;
    if (direction == Side::South)
        return southFacing;
    if (direction == Side::East)
        return eastFacing;
    if (direction == Side::West)
        return westFacing;
    if (direction == Side::Up)
        return upFacing;
    if (direction == Side::Down)
        return downFacing;
    return glm::vec3(0.0f);  // Default/invalid direction
}

void VariantBlock::Neighbours::rotate(Side from, Side to) {
    if (from == to)
        return;

    Neighbours original = *this;

    glm::vec3 axis;
    float angle = getAngle(sideDirection(from), sideDirection(to), axis);

    glm::mat4 rotateMat = glm::rotate(glm::mat4(1.0f), angle, axis);
    auto rotateSide = [&](Side side) -> BlockID {
        glm::vec3 dir = sideDirection(side);
        glm::vec3 rotated = glm::vec3(rotateMat * glm::vec4(dir, 0.0f));

        return original[getSide(glm::round(rotated))];
    };

    auto rotateFacing = [&](Side side) -> glm::vec3 {
        // Get rotated side and rotate that facing dir
        glm::vec3 dir = sideDirection(side);
        glm::vec3 fromRotation = glm::vec3(rotateMat * glm::vec4(dir, 0.0f));

        glm::vec3 facing = original.getFacing(getSide(glm::round(fromRotation)));
        if (glm::length(facing) < 0.01f) {
            return facing;  // No rotation for invalid/zero facings
        }
        glm::vec3 rotated = glm::vec3(rotateMat * glm::vec4(facing, 0.0f));
        // TODO/NOTE: Should check here for rotation mode of the block and if its AxisAlign and if so, glm::abs(rotated)
        return glm::normalize(rotated);
    };

    north = rotateSide(Side::North);
    south = rotateSide(Side::South);
    east = rotateSide(Side::East);
    west = rotateSide(Side::West);
    up = rotateSide(Side::Up);
    down = rotateSide(Side::Down);

    northFacing = rotateFacing(Side::North);
    southFacing = rotateFacing(Side::South);
    eastFacing = rotateFacing(Side::East);
    westFacing = rotateFacing(Side::West);
    upFacing = rotateFacing(Side::Up);
    downFacing = rotateFacing(Side::Down);
}

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