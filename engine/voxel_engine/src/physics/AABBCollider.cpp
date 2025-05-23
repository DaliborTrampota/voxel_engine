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

AABBCollider::AABBCollider(const AABB& aabb, float stepHeight)
    : m_aabb(aabb),
      m_height(aabb.max.y - aabb.min.y),
      m_stepHeight(stepHeight) {
    m_aabbCache.reserve(3 * 3 * 6);  // TODO: should take into account the aabb size
    printf("Cache size: %d\n", m_aabbCache.size());
}

void AABBCollider::setWorld(std::shared_ptr<World> world) {
    m_world = world;
}

void AABBCollider::setAABB(const AABB& aabb) {
    m_aabb = aabb;
}

bool AABBCollider::collide(glm::vec3& velocity, glm::vec3& position) {
    AABB original = m_aabb;
    m_aabb.transform(glm::translate(glm::mat4(1.0f), glm::vec3(position)));
    updateAABBCache(velocity, position);
    bool collided = false;
    for (const AABB& bb : m_aabbCache) {
        for (int i = 0; i < 3; ++i) {
            SweptResult res = swept(velocity, bb);

            if (res.time != 1.0f) {
                collided = true;
                velocity[res.axis] *= res.time;
            }
        }
    }
    m_aabb = original;
    return collided;
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
    for (int i = m_aabb.min.x - s_checkBox.x; i <= m_aabb.max.x + s_checkBox.x; ++i) {
        for (int j = m_aabb.min.y - s_checkBox.y; j <= m_aabb.max.y + s_checkBox.y; ++j) {
            for (int k = m_aabb.min.z - s_checkBox.z; k <= m_aabb.max.z + s_checkBox.z; ++k) {
                glm::ivec3 pos(i, j, k);
                BlockID blockID = m_world->getBlockID(extractChunkCoords(pos), pos);
                if (blockID == INVALID_BLOCK || blockID == Block::air().getID())
                    continue;

                const Block& block = RegistryManager::Blocks().get(blockID);
                if (!block.isSolid())
                    continue;
                AABB aabb = block.geometry()->aabb();
                aabb.transform(glm::translate(glm::mat4(1.0f), glm::vec3(pos)));
                m_aabbCache.push_back(aabb);
            }
        }
    }
    printf("Cache size: %d\n", m_aabbCache.size());
}

AABBCollider::SweptResult AABBCollider::swept(glm::vec3& velocity, const AABB& other) {
    // Broadphase check
    bool positive = glm::all(glm::greaterThan(velocity, glm::vec3(0.0f)));
    AABB rect{
        positive ? m_aabb.min : m_aabb.min + velocity,
        positive ? m_aabb.max + velocity : m_aabb.max,
    };
    if (!rect.intersects(other))
        return {1.0f, -1};

    float dxEntry, dyEntry, dzEntry;
    float dxExit, dyExit, dzExit;

    if (velocity.x > 0.0f) {
        dxEntry = other.min.x - m_aabb.max.x;
        dxExit = other.max.x - m_aabb.min.x;
    } else {
        dxEntry = other.max.x - m_aabb.min.x;
        dxExit = other.min.x - m_aabb.max.x;
    }

    if (velocity.y > 0.0f) {
        dyEntry = other.min.y - m_aabb.max.y;
        dyExit = other.max.y - m_aabb.min.y;
    } else {
        dyEntry = other.max.y - m_aabb.min.y;
        dyExit = other.min.y - m_aabb.max.y;
    }

    if (velocity.z > 0.0f) {
        dzEntry = other.min.z - m_aabb.max.z;
        dzExit = other.max.z - m_aabb.min.z;
    } else {
        dzEntry = other.max.z - m_aabb.min.z;
        dzExit = other.min.z - m_aabb.max.z;
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
