#include "AABBCollider.h"

#include <algorithm>

#include "../Globals.h"
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
      m_stepHeight(stepHeight) {
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


    for (int pass = 0; pass < 3; ++pass) {
        SweptResult bestRes{1.0f, -1};

        for (const AABB& bb : m_aabbCache) {
            SweptResult res = swept(moveStep, bb);
            if (res.time < bestRes.time) {
                bestRes = res;
                hitBBs.clear();
            }

            if (res.time == bestRes.time) {
                hitBBs.push_back(res.bb);
            }
        }

        if (bestRes.axis == -1) {
            break;
        }

        if (bestRes.axis == 1) {
            info.grounded = true;
        }

        if (bestRes.axis != 1 && m_stepHeight > 0.0f) {
            float colHeight = bestRes.bb->max.y;
            float feetHeight = m_aabb->min.y;
            float requiredStepHeight = colHeight - feetHeight;

            if (requiredStepHeight > 0.0f && requiredStepHeight <= m_stepHeight) {
                glm::vec3 horizontalMove{moveStep.x, 0, moveStep.z};

                float stepHeight = tryStepUp(horizontalMove, bestRes.bb);

                if (stepHeight > 0.0f) {
                    info.stepHeight = stepHeight;

                    moveStep.y = stepHeight;  // override gravity for this frame

                    // Clear this axis collision since we stepped over it
                    hitBBs.clear();
                    continue;  // Skip the rest and go to next pass
                }
            }
        }

        info.axis |= 1 << bestRes.axis;
        info.t[bestRes.axis] = bestRes.time;

        float originalAxisMove = moveStep[bestRes.axis];
        float& axisMove = moveStep[bestRes.axis];
        axisMove *= bestRes.time;
        axisMove -= glm::sign(axisMove) * s_epsGap;
        info.correction[bestRes.axis] = originalAxisMove - axisMove;

        info.hitPositions[bestRes.axis].reserve(hitBBs.size());
        for (const AABB* bb : hitBBs) {
            glm::vec3 blockPos = bb->center();
            info.hitPositions[bestRes.axis].push_back(blockPos);
            // auto id = extractChunkCoords(blockPos);
            // info.touchingBlocks.push_back(m_world->getBlockID(id, blockPos));
        }
        hitBBs.clear();
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
                BlockID blockID = m_world->getBlockID({ i, j, k }, true);
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
    // Expand the rectangle slightly to avoid precision issues
    rect.expand(std::numeric_limits<float>::epsilon());  
    return rect;
}

AABBCollider::SweptResult AABBCollider::swept(const glm::vec3& velocity, const AABB& other) const {
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
        return {1.0f, -1, nullptr};  // No collision
    }

    return {entryTime, (entryTime == txEntry) ? 0 : (entryTime == tyEntry) ? 1 : 2, &other};
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

float AABBCollider::tryStepUp(const glm::vec3& horizontalMove, const AABB* collidingBB) {
    if (m_stepHeight <= 0.0f)
        return -1.0f;

    float colHeight = collidingBB->max.y;
    float feetHeight = m_aabb->min.y;
    float stepHeight = colHeight - feetHeight + s_epsGap;
    
    if (stepHeight <= 0.0f || stepHeight > m_stepHeight)
        return -1.0f;

    // Temp move up
    m_aabb->moveAxis(1, stepHeight);

    bool canStep = true;
    
    for (const AABB& aabb : m_aabbCache) {
        SweptResult res = swept(horizontalMove, aabb);

        // Maybe we collide but there is a ledge? Check absolute amount for 1/16th of a block
        if (res.axis != 1 && res.time != 1.0f) {
            float absMove = horizontalMove[res.axis] * res.time;
            if (absMove < 1/16.f) { 
                canStep = false;
                break;
            }
        }

        // Check if there's a ceiling blocking us
        if (m_aabb->intersects(aabb)) {
            canStep = false;
            break;
        }
    }

    m_aabb->moveAxis(1, -stepHeight);

    return canStep ? stepHeight : -1.0f;
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