#pragma once

#include <glm/glm.hpp>
#include <optional>
#include <string>
#include <vector>

#include "Vertex.h"

namespace engine {
    enum FaceTag {
        All = 0,
        Side,

        Top,
        Bottom,
        Right,
        Left,
        Front,
        Back,

        Tag1,
        Tag2,
        Tag3,
        Tag4,
        Tag5,
        Tag6,
        Tag7,
        Tag8,

        Count
    };

    struct Face {
        FaceTag tag;
        std::vector<Vertex> vertices{};
        glm::ivec3 cullDir{0};
        bool cull = false;

        void translate(glm::vec3 t);
        void data(int textureID, int ao);

        void setCull(glm::ivec3 dir) {
            cull = true;
            cullDir = dir;
        }

        static const Face TriangleFace(
            FaceTag tag,
            glm::vec3 p1,
            glm::vec3 p2,
            glm::vec3 p3,
            glm::vec2 uv1,
            glm::vec2 uv2,
            glm::vec2 uv3
        );

        static const Face SquareFace(
            FaceTag tag,
            glm::vec3 start,
            glm::vec3 end,
            glm::vec3 n,
            glm::vec2 uvStart,
            glm::vec2 uvEnd
        );

        static const Face CircleFace(
            FaceTag tag,
            glm::vec3 center,
            float r,
            int segments,
            glm::vec3 n,
            glm::vec2 uvStart,
            glm::vec2 uvEnd
        );

        static const Face CylinderFace(
            FaceTag tag,
            glm::vec3 center,
            float r,
            float h,
            int segments,
            glm::vec3 n,
            glm::vec2 uvStart,
            glm::vec2 uvEnd
        );
    };

}  // namespace engine
