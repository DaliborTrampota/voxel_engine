#pragma once

#include <glm/glm.hpp>


namespace engine {

    class Geometry;

    struct AABB {
        glm::vec3 min;
        glm::vec3 max;

        bool contains(const glm::vec3& point) const;
        bool intersects(const AABB& other) const;
        glm::vec3 center() const;

        AABB transform(const glm::mat4& matrix) const;

        static AABB fromGeometry(const Geometry& geo); 
    };

}