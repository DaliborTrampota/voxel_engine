#include "DirectionalShadowPass.h"

#include <LWGL/buffer/FBO.h>
#include <glad/glad.h>

#include "render/Engine.h"
#include "render/utility/UtilityShaders.h"
#include "scene/Sun.h"

using namespace engine;

DirectionalShadowPass::DirectionalShadowPass(
    glm::ivec2 resolution, const Material* material, const gl::FBO* fbo, const glm::ivec2& shadowRes
)
    : RenderPass(resolution, RenderPass::DirectionalShadow) {
    this->material = material;
    this->fbo = fbo;
    viewportSize = shadowRes;
}

void DirectionalShadowPass::beforeRender(Engine& engine, uint8_t pass) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    fbo->bind();
    fbo->clearDepth(1.0f);

    glm::ivec2 res = viewportSize.value();
    glViewport(0, 0, res.x, res.y);
}

void DirectionalShadowPass::afterRender(Engine& engine, uint8_t pass) {
    glViewport(0, 0, m_resolution.x, m_resolution.y);

    // glm::ivec2 resolution = engine.directionalLightSource()->resolution();
    // applyLinearSamplingBlur(
    //     *fbo,
    //     gl::FBOAttachment::Color,
    //     gl::FBOAttachment::Color + 1,
    //     resolution.x,
    //     resolution.y,
    //     nullptr
    // );
}