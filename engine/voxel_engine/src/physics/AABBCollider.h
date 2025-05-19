#pragma once

#include <glm/glm.hpp>
#include <memory>

namespace engine {

    class World;
    struct AABB;

    class AABBCollider {
      public:
        AABBCollider(std::shared_ptr<AABB> aabb, float stepHeight = 0.0f);

        /// @brief Sets the world for this collider.
        void setWorld(std::shared_ptr<World> world);

        /// @brief Sets the AABB that will be checked for collisions against the world.
        void setAABB(std::shared_ptr<AABB> aabb);

        /// @brief Checks if the AABB collides with any blocks in the world.
        /// @param velocity The velocity of the AABB. After the function call, this will be modified to
        ///                reflect the collision response. (eg. stopping movement in a obscured direction)
        /// @param position The position of the AABB. After the function call, this will be modified to
        ///                reflect the collision response. (eg. stepping up)
        /// @note This function should be called for each axis, so 3 times for x, y and z.
        bool collide(glm::vec3& velocity, glm::vec3& position) const;

      private:
        std::shared_ptr<World> m_world = nullptr;
        std::shared_ptr<AABB> m_aabb;
        float m_stepHeight;

        std::vector<AABB> m_aabbCache;  // Cached world AABBs around the AABB.
        glm::vec3 m_lastPosition;  // The last position of the AABB thus where the aabbCache is generated.
        glm::vec3 m_cacheDirection;  // The direction in which the AABB cache is generated.

        void updateAABBCache();
    };

}  // namespace engine