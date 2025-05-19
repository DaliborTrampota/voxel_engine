#include "AABB.h"

#include "block/Geometry.h"

using namespace engine;

bool AABB::contains(const glm::vec3& point) const {
    return min.x < point.x && point.x <= max.x && min.y < point.y && point.y <= max.y &&
           min.z < point.z && point.z <= max.z;
}

bool AABB::intersects(const AABB& other) const {
    return min.x < other.max.x && min.y < other.max.y && min.z < other.max.z &&
           max.x >= other.min.x && max.y >= other.min.y && max.z >= other.min.z;
}

glm::vec3 AABB::center() const {
    return (min + max) / 2.0f;
}

AABB AABB::transform(const glm::mat4& matrix) const {
    return {
        glm::vec3(matrix * glm::vec4(min, 1.0f)),
        glm::vec3(matrix * glm::vec4(max, 1.0f))
    };
}

AABB AABB::fromGeometry(const Geometry& geo) {
    AABB aabb;
    aabb.min = glm::vec3(geo.faces()[0].vertices[0].pos);
    aabb.max = aabb.min;

    for (const auto& face : geo.faces()) {
        for (const auto& vertex : face.vertices) {
            aabb.min = glm::min(aabb.min, glm::vec3(vertex.pos));
            aabb.max = glm::max(aabb.max, glm::vec3(vertex.pos));
        }
    }

    return aabb;
}
