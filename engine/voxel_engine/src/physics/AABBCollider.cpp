#include "AABBCollider.h"

#include "AABB.h"
#include "CoordUtils.h"
#include "data/RegistryManager.h"
#include "level/World.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace engine;

AABBCollider::AABBCollider(std::shared_ptr<AABB> aabb, float stepHeight)
    : m_aabb(aabb),
      m_height(aabb->max.y - aabb->min.y),
      m_stepHeight(stepHeight) {
    m_aabbCache.reserve(3 * 3 * 6);  // TODO: should take into account the aabb size
}

void AABBCollider::setWorld(std::shared_ptr<World> world) {
    m_world = world;
}

void AABBCollider::setAABB(std::shared_ptr<AABB> aabb) {
    m_aabb = aabb;
}

CollisionInfo AABBCollider::collide(glm::vec3& velocity, glm::vec3& position) {
    updateAABBCache(velocity, position);
    CollisionInfo info;
    std::vector<const AABB*> hitBBs{};
    glm::vec3 displacement{0};

    for (int i = 0; i < 3; ++i) {
        float bestTime = 1.f;

        for (const AABB& bb : m_aabbCache) {
            glm::vec3 axisVel{0, 0, 0};
            axisVel[i] = velocity[i];

            float t = swept(axisVel, bb).time;
            if (t == 1.0f) {
                continue;
            } else if (t == bestTime) {
                hitBBs.push_back(&bb);
            } else if(t < bestTime) {
                bestTime = t;
                hitBBs.clear();
                hitBBs.push_back(&bb);
            }
        }

        if (bestTime != 1.0f) {
            static const float eps = 0.0001f;
            
            info.axis ^= i + 1;
            info.correction[i] = glm::sign(velocity[i]) * eps;

            info.hitPositions[i].reserve(hitBBs.size());
            for (const AABB* bb : hitBBs) {
                glm::vec3 blockPos = bb->center();
                info.hitPositions[i].push_back(blockPos);

                printf("Colliding with %d bbs\n", hitBBs.size());

                auto id = extractChunkCoords(blockPos);
                info.touchingBlocks.push_back(m_world->getBlockID(id, blockPos));
            }

            velocity[i] *= glm::min(bestTime, 1.0f - std::numeric_limits<float>::epsilon());
            hitBBs.clear();
        }
    }
    return info;
}

void AABBCollider::updateAABBCache(const glm::vec3& velocity, const glm::vec3& position) {
    glm::ivec3 newPos = floorToInt(position);
    if (m_lastPosition == newPos)
        return;

    m_lastPosition = newPos;
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
    m_aabbCache.clear();
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
}

AABB broadphaseRect(const glm::vec3 velocity, const AABB& bb) {
    return {
        glm::min(bb.min, bb.min + velocity),
        glm::max(bb.max, bb.max + velocity),
    };
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

float engine::AABBCollider::swept1D(int axis, float velocity, const AABB& other) {
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
