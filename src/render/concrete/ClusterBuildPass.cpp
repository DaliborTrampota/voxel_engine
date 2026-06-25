#include "ClusterBuildPass.h"
#include <LWGL/buffer/SSBO.h>

#include "scene/Camera.h"
#include "scene/PointLightManager.h"

#include <glad/glad.h>


using namespace engine;

ClusterBuildPass::ClusterBuildPass(
    glm::ivec2 resolution, Camera* camera, PointLightManager* pointLightManager
)
    : RenderPass(resolution, RenderPass::ClusterBuildPass),
      m_camera(camera),
      m_pointLightManager(pointLightManager),
      m_clusters(GL_STATIC_DRAW),
      m_compute("resources/shaders/light/ClusterBuild.comp", true) {
    m_maxClusters = ClusterGridSize.x * ClusterGridSize.y * ClusterGridSize.z;
    m_groups = (m_maxClusters + LocalSize - 1) / LocalSize;
    m_compute.setConstant("OMNI_MAX_LIGHTS_PER_CLUSTER", (int)MaxLightsPerCluster);
    m_compute.setConstant("OMNI_CLUSTER_COUNT", (int)m_maxClusters);
    m_compute.setConstant("OMNI_LOCAL_SIZE", (int)LocalSize);
    m_compute.compile();
    m_clusters.create(m_maxClusters);
    m_lightIndices.create(m_maxClusters * MaxLightsPerCluster);
    m_clusterGrid.create(m_maxClusters);
    m_atomicCounter.create(1);
    subdivideFrustum();
}


void ClusterBuildPass::beforeRender(Engine& engine, uint8_t pass) {
    m_pointLightManager->bindLights(1);
    m_compute.setUInt("lightCount", m_pointLightManager->lightCount());
    m_compute.setMat4("view", m_camera->getView());

    m_clusterGrid.bind(2);
    m_lightIndices.bind(3);
    m_clusters.bind(4);
    m_atomicCounter.bind(5);
    m_atomicCounter.clear();
    m_atomicCounter.add(0);
    m_atomicCounter.upload();

    m_compute.dispatch(m_groups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}


void ClusterBuildPass::resize(glm::ivec2 resolution) {
    m_resolution = resolution;
    subdivideFrustum();
}

void ClusterBuildPass::subdivideFrustum() {
    m_clusters.clear();

    glm::mat4 proj = m_camera->getProjection();
    auto unproject = [&](float ndcX, float ndcY, float z) -> glm::vec3 {
        return {
            ndcX / proj[0][0] * glm::abs(z),
            ndcY / proj[1][1] * glm::abs(z),
            z,
        };
    };

    for (int z = 0; z < ClusterGridSize.z; z++) {
        for (int y = 0; y < ClusterGridSize.y; y++) {
            for (int x = 0; x < ClusterGridSize.x; x++) {
                glm::vec2 ndcMin = glm::vec2(
                                       static_cast<float>(x) / ClusterGridSize.x,
                                       static_cast<float>(y) / ClusterGridSize.y
                                   ) * 2.0f -
                                   1.0f;
                glm::vec2 ndcMax = glm::vec2(
                                       static_cast<float>(x + 1) / ClusterGridSize.x,
                                       static_cast<float>(y + 1) / ClusterGridSize.y
                                   ) * 2.0f -
                                   1.0f;

                float near =
                    -m_camera->nearPlane() * glm::pow(
                                                 m_camera->farPlane() / m_camera->nearPlane(),
                                                 static_cast<float>(z) / ClusterGridSize.z
                                             );
                float far =
                    -m_camera->nearPlane() * glm::pow(
                                                 m_camera->farPlane() / m_camera->nearPlane(),
                                                 static_cast<float>(z + 1) / ClusterGridSize.z
                                             );


                glm::vec3 corners[8] = {
                    unproject(ndcMin.x, ndcMin.y, near),
                    unproject(ndcMax.x, ndcMin.y, near),
                    unproject(ndcMin.x, ndcMax.y, near),
                    unproject(ndcMax.x, ndcMax.y, near),
                    unproject(ndcMin.x, ndcMin.y, far),
                    unproject(ndcMax.x, ndcMin.y, far),
                    unproject(ndcMin.x, ndcMax.y, far),
                    unproject(ndcMax.x, ndcMax.y, far),
                };

                glm::vec3 aabbMin = corners[0];
                glm::vec3 aabbMax = corners[0];

                for (int c = 1; c < 8; c++) {
                    aabbMin = glm::min(aabbMin, corners[c]);
                    aabbMax = glm::max(aabbMax, corners[c]);
                }

                m_clusters.add(
                    ClusterAABB{.min = glm::vec4(aabbMin, 0.0f), .max = glm::vec4(aabbMax, 0.0f)}
                );
            }
        }
    }
    m_clusters.upload();
}

void ClusterBuildPass::bindForShading(
    unsigned int clusterGridBinding, unsigned int lightIndicesBinding
) const {
    m_clusterGrid.bind(clusterGridBinding);
    m_lightIndices.bind(lightIndicesBinding);
}