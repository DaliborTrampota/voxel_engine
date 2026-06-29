#pragma once

#include <glm/glm.hpp>


namespace engine {

    class Geometry;
    struct Plane;
    struct Sphere;

    struct AABB {
        glm::vec3 min;
        glm::vec3 max;

        bool contains(const glm::vec3& point) const;
        bool intersects(const AABB& other) const;
        bool intersects(const Plane& plane) const;
        bool intersects(const Sphere& sphere) const;
        bool isOutsidePlane(const Plane& plane) const;
        glm::vec3 center() const;

        void transform(const glm::mat4& matrix);
        void move(const glm::vec3& offset);
        void moveAxis(int axis, float amount);
        void position(const glm::vec3& pos);
        void expand(float amount);

        static AABB fromGeometry(const Geometry& geo);
        static AABB fromPoints(const std::vector<glm::vec3>& points);
    };

}  // namespace engine