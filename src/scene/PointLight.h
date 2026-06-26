#pragma once

#include <cstdint>
#include <glm/glm.hpp>

namespace engine {

    struct PointLight {
        using ID = uint32_t;

        alignas(16) glm::vec3 position;
        alignas(16) glm::vec3 color;
        float intensity;
        float radius;
        ID shadowIndex = NoShadow;
        ID id = NoShadow;  // CPU-side stable handle; not read by GPU (lies past shadowIndex)
        // std430 array stride requires struct size to be multiple of 16 (48 bytes)
        uint32_t _pad;

        static constexpr ID NoShadow = 0xFFFFFFFF;
        static constexpr ID NoID = 0xFFFFFFFF;
    };

    static_assert(sizeof(PointLight) == 48, "PointLight size must match std430 GPU layout");
}  // namespace engine