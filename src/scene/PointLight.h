#pragma once

#include <cstdint>
#include <glm/glm.hpp>

namespace engine {

    struct PointLight {
        alignas(16) glm::vec3 position;
        alignas(16) glm::vec3 color;
        float intensity;
        float radius;
        uint32_t shadowIndex = NoShadow;
        uint32_t id = 0;  // CPU-side stable handle; not read by GPU (lies past shadowIndex)
        // std430 array stride requires struct size to be multiple of 16 (48 bytes)
        uint32_t _pad;

        static constexpr uint32_t NoShadow = 0xFFFFFFFF;
    };

    static_assert(sizeof(PointLight) == 48, "PointLight size must match std430 GPU layout");
}  // namespace engine