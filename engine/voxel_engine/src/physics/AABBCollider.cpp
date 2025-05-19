#include "AABBCollider.h"

#include "AABB.h"
#include "level/World.h"

using namespace engine;

AABBCollider::AABBCollider(std::shared_ptr<AABB> aabb, float stepHeight)
    : m_aabb(aabb),
      m_stepHeight(stepHeight) {
    m_aabbCache.reserve(3 * 3 * 6); // TODO: should take into account the aabb size
}

void AABBCollider::setWorld(std::shared_ptr<World> world) {
    m_world = world;
}

void AABBCollider::setAABB(std::shared_ptr<AABB> aabb) {
    m_aabb = aabb;
}

bool AABBCollider::collide(glm::vec3& velocity, glm::vec3& position) const {
    
}

void AABBCollider::updateAABBCache() {
    
}
