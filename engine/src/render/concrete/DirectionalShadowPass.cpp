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
    glm::ivec2 res = engine.window()->windowSize();
    glViewport(0, 0, res.x, res.y);

    glm::ivec2 resolution = engine.directionalLightSource()->resolution();
    applyLinearSamplingBlur(
        *fbo,
        gl::FBOAttachment::Color,
        gl::FBOAttachment::Color + 1,
        resolution.x,
        resolution.y,
        nullptr
    );
}


std::unique_ptr<RenderPass> DirectionalShadowPass::create(
    const Material* material, const gl::FBO* fbo, const glm::ivec2& resolution
) {
    return std::make_unique<DirectionalShadowPass>(
        DirectionalShadowPass(RenderPass::DirectionalShadow, material, fbo, resolution)
    );
}