#pragma once

#include <glm/glm.hpp>

namespace engine {
    class Block;
    class World;
    class Chunk;
    using UnaryPredicate = std::function<bool(Block)>;

    struct DDAResult {
        glm::vec3 position;
        glm::vec3 face;
        const Block& block;
        float distance;
        const Chunk* chunk;
    };

    struct Ray {
        glm::vec3 origin;
        glm::vec3 direction;
    };
    struct Triangle {
        glm::vec3 p1, p2, p3;
    };

    DDAResult DDA(
        const World& world, glm::vec3 start, glm::vec3 direction, float length, UnaryPredicate pred
    );

    /// @brief Compute the intersection of a ray and a triangle
    /// @param ray The ray to intersect
    /// @param t The triangle to intersect
    /// @param uOut The u coordinate of the intersection
    /// @param vOut The v coordinate of the intersection
    /// @return The t parameter of the intersection. ray.origin + ray.direction * t or std::numeric_limits<float>::infinity() if no intersection
    /// @note This function uses the Moller-Trumbor algorithm
    float rayTriangleIntersection(Ray& ray, Triangle& t, float& uOut, float& vOut);
}  // namespace engine