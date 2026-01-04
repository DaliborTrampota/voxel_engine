#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Vertex.h"

namespace engine {
    enum class FaceTag {
        All = 0,
        Side,

        Front,
        Right,
        Back,
        Left,
        Top,
        Bottom,

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
        glm::ivec3 cullDir = {0, 0, 0};
        bool cull = false;
        bool doubleSided = false;  // When true, renders both sides (disables backface culling)

        void translate(glm::vec3 t);
        void rotate(glm::vec3 axis, float angle);
        void data(int textureID, int ao);

        void setCull(glm::ivec3 dir) {
            cull = true;
            cullDir = dir;
        }

        void setDoubleSided(bool enabled = true) {
            doubleSided = enabled;
            cull = false;
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

        static const Face RectangleFace(
            FaceTag tag,
            glm::vec3 start,
            glm::vec3 end,
            float length,
            glm::vec3 n,
            glm::vec2 uvStart,
            glm::vec2 uvEnd
        );
    };

    constexpr FaceTag IterateFaces[6] = {
        FaceTag::Front, FaceTag::Right, FaceTag::Back, FaceTag::Left, FaceTag::Top, FaceTag::Bottom
    };

    constexpr FaceTag IterateXZFaces[4] = {
        FaceTag::Front,
        FaceTag::Right,
        FaceTag::Back,
        FaceTag::Left,
    };

    constexpr FaceTag IterateYFaces[2] = {FaceTag::Top, FaceTag::Bottom};

}  // namespace engine
