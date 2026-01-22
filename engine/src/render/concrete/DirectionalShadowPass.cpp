#include "DirectionalShadowPass.h"

#include <LWGL/buffer/FBO.h>
#include <glad/glad.h>


#include "render/Engine.h"
#include "render/utility/UtilityShaders.h"
#include "scene/Sun.h"

using namespace engine;

void DirectionalShadowPass::beforeRender(Engine& engine, uint8_t pass) {
    fbo->bind();
    fbo->clearActive({1.f, 1.f, 1.f, 1.f}, 1.0f);

    glm::ivec2 res = viewportSize.value();
    glViewport(0, 0, res.x, res.y);
}

void DirectionalShadowPass::afterRender(Engine& engine, uint8_t pass) {

    glm::ivec2 resolution = engine.directionalLightSource()->resolution();
    applyLinearSamplingBlur(
        *fbo,
        gl::FBOAttachment::Color,
        gl::FBOAttachment::Color + 1,
        resolution.x,
        resolution.y,
        nullptr
    );
    glViewport(0, 0, m_resolution.x, m_resolution.y);
}


std::unique_ptr<RenderPass> DirectionalShadowPass::create(
    glm::ivec2 resolution, const Material* material, const gl::FBO* fbo, const glm::ivec2& shadowRes
) {
    return std::make_unique<DirectionalShadowPass>(
        DirectionalShadowPass(resolution, RenderPass::DirectionalShadow, material, fbo, shadowRes)
    );
}