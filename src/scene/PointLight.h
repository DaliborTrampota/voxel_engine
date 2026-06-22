#pragma once

#include <glm/glm.hpp>

namespace engine {

    struct PointLight {
        alignas(16) glm::vec3 position;
        alignas(16) glm::vec3 color;
        float intensity;
        float radius;
        uint32_t shadowIndex = NoShadow;
        // std430 array stride requires struct size to be multiple of 16 (48 bytes)
        uint32_t _pad[2];

        static constexpr uint32_t NoShadow = 0xFFFFFFFF;
    };

    static_assert(sizeof(PointLight) == 48, "PointLight size must match std430 GPU layout");
}  // namespace engine