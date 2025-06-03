#include "AABBCollider.h"

#include "AABB.h"
#include "CoordUtils.h"
#include "data/RegistryManager.h"
#include "level/World.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/norm.hpp>

using namespace engine;

AABBCollider::AABBCollider(std::shared_ptr<AABB> aabb, float stepHeight, float groundedHeight)
    : m_aabb(aabb),
      m_lastPosition(0.f),
      m_cacheDirection(0.f),
      m_height(aabb->max.y - aabb->min.y),
      m_stepHeight(stepHeight),
      m_groundedHeight(groundedHeight) {
    if (m_height <= 0.0f)
        throw std::runtime_error("AABBCollider: AABB height must be greater than 0.");

    AABB bb(m_aabb->min - s_checkBox, m_aabb->max + s_checkBox);
    int volume = glm::compMul(bb.max - bb.min);
    m_aabbCache.reserve(volume - (m_aabb->max.y - m_aabb->min.y));
}

void AABBCollider::setWorld(std::shared_ptr<World> world) {
    m_world = world;
}

void AABBCollider::setAABB(std::shared_ptr<AABB> aabb) {
    m_aabb = aabb;
}

CollisionInfo AABBCollider::collide(glm::vec3& moveStep, const glm::vec3& position) {
    updateAABBCache(moveStep, position);

    CollisionInfo info;
    std::vector<const AABB*> hitBBs{};
    glm::vec3 displacement{0};

    for (int pass = 0; pass < 3; ++pass) {
        float bestTime = 1.f;
        int axis = -1;

        for (const AABB& bb : m_aabbCache) {
            SweptResult res = swept(moveStep, bb);
            if (bb.intersects(*m_aabb)) {
                printf(
                    "AABBCollider: AABB cache intersects with player AABB, this should not "
                    "happen! %.4f\n",
                    res.time
                );
                printf(
                    "AABB from (%.2f, %.2f, %.2f) to (%.2f, %.2f, %.2f)\n",
                    bb.min.x,
                    bb.min.y,
                    bb.min.z,
                    bb.max.x,
                    bb.max.y,
                    bb.max.z
                );
                printf(
                    "Player AABB from (%.2f, %.2f, %.2f) to (%.2f, %.2f, %.2f)\n",
                    m_aabb->min.x,
                    m_aabb->min.y,
                    m_aabb->min.z,
                    m_aabb->max.x,
                    m_aabb->max.y,
                    m_aabb->max.z
                );
            }

            if (res.time == 1.0f) {
                if (res.axis == 1 && moveStep[res.axis] <= 0.0f) {  // y axis
                    AABB extendedBB = bb;
                    extendedBB.max.y += m_groundedHeight;
                    if (extendedBB.intersects(*m_aabb))
                        info.grounded = true;
                }
                continue;
            } else if (res.time == bestTime) {
                hitBBs.push_back(&bb);
            } else if (res.time < bestTime) {
                bestTime = res.time;
                axis = res.axis;
                hitBBs.clear();
                hitBBs.push_back(&bb);

                if (res.axis == 1)  // y axis
                    info.grounded = bb.max.y + m_groundedHeight >= m_aabb->min.y;
            }
        }

        if (bestTime != 1.0f) {
            info.axis |= 1 << axis;
            info.correction[axis] =
                glm::sign(moveStep[axis]) * std::numeric_limits<float>::epsilon() * 64.0f;
            info.hitPositions[axis].reserve(hitBBs.size());
            info.t[axis] = bestTime;

            moveStep[axis] *= bestTime;

            for (const AABB* bb : hitBBs) {
                glm::vec3 blockPos = bb->center();
                info.hitPositions[axis].push_back(blockPos);
                // auto id = extractChunkCoords(blockPos);
                // info.touchingBlocks.push_back(m_world->getBlockID(id, blockPos));
            }

            hitBBs.clear();
        }
    }
    return info;
}

void AABBCollider::updateAABBCache(const glm::vec3& velocity, const glm::vec3& position) {
    glm::ivec3 newPos = glm::floor(position);

    if (m_lastPosition == newPos)
        return;

    m_lastPosition = newPos;
    m_aabbCache.clear();
    // clang-format off
    for (int i = glm::floor(m_aabb->min.x - s_checkBox.x); i <= glm::ceil(m_aabb->max.x + s_checkBox.x); ++i) {
        for (int j = glm::floor(m_aabb->min.y - s_checkBox.y); j <= glm::ceil(m_aabb->max.y + s_checkBox.y); ++j) {
            for (int k = glm::floor(m_aabb->min.z - s_checkBox.z); k <= glm::ceil(m_aabb->max.z + s_checkBox.z); ++k) {
                glm::ivec3 pos(i, j, k);
                auto chID = extractChunkCoords(pos);
                BlockID blockID = m_world->getBlockID(chID, pos);
                if (blockID == INVALID_BLOCK || blockID == Block::air().getID())
                    continue;

                const Block& block = RegistryManager::Blocks().get(blockID);
                if (!block.isSolid())
                    continue;
                AABB aabb = block.geometry()->aabb();
                aabb.transform(glm::translate(glm::mat4(1.0f), glm::vec3(i, j, k)));
                m_aabbCache.push_back(aabb);
            }
        }
    }
    // clang-format off
}

AABB broadphaseRect(const glm::vec3 velocity, const AABB& bb) {
    AABB rect{
        glm::min(bb.min, bb.min + velocity),
        glm::max(bb.max, bb.max + velocity),
    };
    rect.expand(std::numeric_limits<float>::epsilon());  // Expand the rectangle slightly to avoid precision issues
    return rect;
}

AABBCollider::SweptResult AABBCollider::swept(glm::vec3& velocity, const AABB& other) {
    // Broadphase check
    if (!broadphaseRect(velocity, *m_aabb).intersects(other))
        return {1.0f, -1};

    float dxEntry, dyEntry, dzEntry;
    float dxExit, dyExit, dzExit;

    if (velocity.x > 0.0f) {
        dxEntry = other.min.x - m_aabb->max.x;
        dxExit = other.max.x - m_aabb->min.x;
    } else {
        dxEntry = other.max.x - m_aabb->min.x;
        dxExit = other.min.x - m_aabb->max.x;
    }

    if (velocity.y > 0.0f) {
        dyEntry = other.min.y - m_aabb->max.y;
        dyExit = other.max.y - m_aabb->min.y;
    } else {
        dyEntry = other.max.y - m_aabb->min.y;
        dyExit = other.min.y - m_aabb->max.y;
    }

    if (velocity.z > 0.0f) {
        dzEntry = other.min.z - m_aabb->max.z;
        dzExit = other.max.z - m_aabb->min.z;
    } else {
        dzEntry = other.max.z - m_aabb->min.z;
        dzExit = other.min.z - m_aabb->max.z;
    }

    float txEntry, tyEntry, tzEntry;
    float txExit, tyExit, tzExit;


    if (std::abs(velocity.x) <= std::numeric_limits<float>::epsilon()) {
        txEntry = -std::numeric_limits<float>::infinity();
        txExit = std::numeric_limits<float>::infinity();
    } else {
        txEntry = dxEntry / velocity.x;
        txExit = dxExit / velocity.x;
    }

    if (std::abs(velocity.y) <= std::numeric_limits<float>::epsilon()) {
        tyEntry = -std::numeric_limits<float>::infinity();
        tyExit = std::numeric_limits<float>::infinity();
    } else {
        tyEntry = dyEntry / velocity.y;
        tyExit = dyExit / velocity.y;
    }

    if (std::abs(velocity.z) <= std::numeric_limits<float>::epsilon()) {
        tzEntry = -std::numeric_limits<float>::infinity();
        tzExit = std::numeric_limits<float>::infinity();
    } else {
        tzEntry = dzEntry / velocity.z;
        tzExit = dzExit / velocity.z;
    }

    float entryTime = std::max({txEntry, tyEntry, tzEntry});
    float exitTime = std::min({txExit, tyExit, tzExit});

    if (entryTime > exitTime || entryTime < 0.0f || entryTime > 1.0f) {
        return {1.0f, -1};  // No collision
    }

    return {entryTime, (entryTime == txEntry) ? 0 : (entryTime == tyEntry) ? 1 : 2};
}

float AABBCollider::swept1D(int axis, float velocity, const AABB& other) {
    // Broadphase check
    //if (!broadphaseRect(velocity, *m_aabb).intersects(other))
    //    return 1.0f;

    float dEntry;
    float dExit;

    if (velocity > 0.0f) {
        dEntry = other.min[axis] - m_aabb->max[axis];
        dExit = other.max[axis] - m_aabb->min[axis];
    } else {
        dEntry = other.max[axis] - m_aabb->min[axis];
        dExit = other.min[axis] - m_aabb->max[axis];
    }

    float entryTime;
    float exitTime;


    if (std::abs(velocity) <= std::numeric_limits<float>::epsilon()) {
        entryTime = -std::numeric_limits<float>::infinity();
        exitTime = std::numeric_limits<float>::infinity();
    } else {
        entryTime = dEntry / velocity;
        exitTime = dExit / velocity;
    }

    if (entryTime > exitTime || entryTime < 0.0f || entryTime > 1.0f) {
        return 1.0f;  // No collision
    }

    return entryTime;
}


// for updateAABB

// m_cacheDirection = newPos + velocity;

// for (const glm::ivec3& feetPos : traceLine(position, position + velocity)) {
//     int h = 0;
//     while (h <= m_height) {
//         glm::ivec3 pos = feetPos;
//         pos.y += h + 1 > m_height ? m_height : h;

//         BlockID blockID = m_world->getBlockID(extractChunkCoords(pos), pos);
//         if(blockID == INVALID_BLOCK || blockID == Block::air().getID())
//             continue;

//         const Block& block = RegistryManager::Blocks().get(blockID)
//         if (!block.isSolid())
//             continue;

//         AABB aabb = block.geometry()->aabb();
//         aabb.transform(glm::translate(glm::mat4(1.0f), glm::vec3(pos)));
//         m_aabbCache.push_back(std::move(aabb));

//         h += 1;
//     }
// }