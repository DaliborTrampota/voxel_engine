#include "AABB.h"

#include "block/Geometry.h"

using namespace engine;

bool AABB::contains(const glm::vec3& point) const {
    return min.x < point.x && point.x <= max.x && min.y < point.y && point.y <= max.y &&
           min.z < point.z && point.z <= max.z;
}

bool AABB::intersects(const AABB& other) const {
    return min.x < other.max.x && min.y < other.max.y && min.z < other.max.z &&
           max.x > other.min.x && max.y > other.min.y && max.z > other.min.z;
}

glm::vec3 AABB::center() const {
    return (min + max) / 2.0f;
}

void AABB::transform(const glm::mat4& matrix) {
    min = glm::vec3(matrix * glm::vec4(min, 1.0f));
    max = glm::vec3(matrix * glm::vec4(max, 1.0f));
}

void AABB::move(const glm::vec3& offset) {
    min += offset;
    max += offset;
}

void AABB::moveAxis(int axis, float amount) {
    if (axis == 0) {
        min.x += amount;
        max.x += amount;
    } else if (axis == 1) {
        min.y += amount;
        max.y += amount;
    } else if (axis == 2) {
        min.z += amount;
        max.z += amount;
    }
}

void AABB::position(const glm::vec3& pos) {
    glm::vec3 center = this->center();
    center.y = min.y;
    glm::vec3 offset = pos - center;
    move(offset);
}

void AABB::expand(float amount) {
    min -= glm::vec3(amount);
    max += glm::vec3(amount);
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

AABB AABB::fromPoints(const std::vector<glm::vec3>& points) {
    AABB aabb;
    aabb.min = glm::vec3(points[0]);
    aabb.max = aabb.min;
    for (const auto& point : points) {
        aabb.min = glm::min(aabb.min, point);
        aabb.max = glm::max(aabb.max, point);
    }
    return aabb;
}
