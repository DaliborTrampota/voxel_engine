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

        void transform(const glm::mat4& matrix);
        void move(const glm::vec3& offset);
        void moveAxis(int axis, float amount);
        void position(const glm::vec3& pos);
        void expand(float amount);

        static AABB fromGeometry(const Geometry& geo);
    };

}  // namespace engine