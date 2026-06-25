#pragma once

#include "../RenderPass.h"
#include "physics/AABB.h"

#include <LWGL/buffer/SSBO.h>
#include <LWGL/render/ComputeProgram.h>

namespace engine {

    class Camera;
    class PointLightManager;

    class ClusterBuildPass : public RenderPass {
      public:
        ClusterBuildPass(
            glm::ivec2 resolution, Camera* camera, PointLightManager* pointLightManager
        );

        void beforeRender(Engine& engine, uint8_t pass) override;
        void afterRender(Engine& engine, uint8_t pass) override {}

        void resize(glm::ivec2 resolution) override;

        void bindForShading(
            unsigned int clusterGridBinding = 2, unsigned int lightIndicesBinding = 3
        ) const;

      private:
        static constexpr glm::ivec3 ClusterGridSize = {16, 9, 24};
        static constexpr uint32_t MaxLightsPerCluster = 128;
        static constexpr uint32_t LocalSize = 64;

        Camera* m_camera;
        PointLightManager* m_pointLightManager = nullptr;
        gl::ComputeProgram m_compute;

        struct ClusterGrid {
            uint32_t offset;
            uint32_t count;
        };

        struct ClusterAABB {
            glm::vec4 min;
            glm::vec4 max;
        };

        gl::SSBO<ClusterAABB> m_clusters;
        uint32_t m_maxClusters;
        uint32_t m_groups;
        // GPU-only buffers
        gl::SSBO<uint32_t> m_lightIndices;
        gl::SSBO<ClusterGrid> m_clusterGrid;
        // TODO replace by GL_ATOMIC_COUNTER_BUFFER
        gl::SSBO<uint32_t> m_atomicCounter;


        void subdivideFrustum();  // computeClusters
    };
}  // namespace engine