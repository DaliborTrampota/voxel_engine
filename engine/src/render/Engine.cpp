#include "Engine.h"

#include <algorithm>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include <LWGL/buffer/Attributes.h>
#include <LWGL/buffer/FBO.h>

#include "RenderPass.h"
#include "data/RegistryManager.h"
#include "level/Chunk.h"
#include "level/World.h"
#include "render/Material.h"
#include "render/Renderable.h"
#include "scene/Camera.h"
#include "scene/Sun.h"
#include "scene/Updateable.h"
#include "utility/UtilityShaders.h"

#include "concrete/DirectionalShadowPass.h"
#include "concrete/ScenePass.h"
#include "concrete/TransparentPass.h"


using namespace engine;

Engine::Engine(std::unique_ptr<Window> window) : m_window(std::move(window)) {
    RegistryManager::Blocks().add(Block::air(), "air");
    RegistryManager::Blocks().add(Block::multiblock(), "multiblock");
    // RegistryManager::Blocks().add(Block(2, Layers::Any, nullptr), "reserved_block_2");
    // RegistryManager::Blocks().add(Block(3, Layers::Any, nullptr), "reserved_block_3");
    // RegistryManager::Blocks().add(Block(4, Layers::Any, nullptr), "reserved_block_4");
    // RegistryManager::Blocks().add(Block(5, Layers::Any, nullptr), "reserved_block_5");
    // RegistryManager::Blocks().add(Block(6, Layers::Any, nullptr), "reserved_block_6");
    // RegistryManager::Blocks().add(Block(7, Layers::Any, nullptr), "reserved_block_7");
    // RegistryManager::Blocks().add(Block(8, Layers::Any, nullptr), "reserved_block_8");
}

void Engine::submitRender(RenderContext&& ctx, bool immediate) {
    if (!immediate) {
        m_renderQueue.push_back(std::move(ctx));
        return;
    }

    for (const auto& pass : m_renderPasses) {
        if ((ctx.passMask & pass->id()) != 0) {
            for (uint8_t subPass = 0; subPass < pass->passes(); subPass++) {
                pass->beforeRender(*this, subPass);
                render(ctx, pass.get());
                pass->afterRender(*this, subPass);
            }
        }
    }
}

void Engine::submitRender(GroupRenderContext&& ctx, bool immediate) {
    if (!immediate) {
        m_renderQueue.push_back(std::move(ctx));
        return;
    }

    for (const auto& pass : m_renderPasses) {
        if ((ctx.passMask & pass->id()) != 0) {
            for (uint8_t subPass = 0; subPass < pass->passes(); subPass++) {
                pass->beforeRender(*this, subPass);
                render(ctx, pass.get());
                pass->afterRender(*this, subPass);
            }
        }
    }
}

void Engine::flush() {
    //printf("Flush %zu\n", m_renderQueue.size());
    for (const auto& pass : m_renderPasses) {
        for (uint8_t subPass = 0; subPass < pass->passes(); subPass++) {
            pass->beforeRender(*this, subPass);

            for (auto& ctxVariant : m_renderQueue) {
                std::visit(
                    [this, &pass](auto& ctx) {
                        if ((ctx.passMask & pass->id()) == 0)
                            return;
                        this->render(ctx, pass.get());
                    },
                    ctxVariant
                );
            }

            pass->afterRender(*this, subPass);
        }
    }

    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glBindProgramPipeline(0);
    m_renderQueue.clear();
}

void Engine::registerRenderPass(std::unique_ptr<RenderPass> pass, uint8_t position) {
    position = glm::min(position, static_cast<uint8_t>(m_renderPasses.size()));
    m_renderPasses.insert(m_renderPasses.begin() + position, std::move(pass));
}

void Engine::registerDefaultRenderPasses() {
    registerRenderPass(TransparentPass::create(), 0);
    registerRenderPass(ScenePass::create(), 1);

    if (m_directionalLightSource) {
        registerRenderPass(
            DirectionalShadowPass::create(
                m_directionalLightSource->shadowMaterial(),
                m_directionalLightSource->shadowFBO(),
                m_directionalLightSource->resolution()
            ),
            0
        );
    }
}


void Engine::gameloop() {
    //TODO
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    //glDisable(GL_CULL_FACE);
    //glDisable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);


    float deltaTime = 0.0f;
    float lastFrame = m_window->time();
    while (!m_window->shouldClose()) {
        float currentFrame = m_window->time();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        beginFrame();

        beforeRender();
        render(deltaTime);
        flush();
        afterRender();

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
            printf("OpenGL error: %d\n", err);

        endFrame();
    }
}

void Engine::render(RenderContext& ctx, const RenderPass* renderPass) const {
    size_t n = ctx.attributes->length();
    if (n == 0)
        return;

    // Apply override if set
    const Material* material = renderPass->material ? renderPass->material : ctx.material;
    const gl::FBO* fbo = renderPass->fbo ? renderPass->fbo : ctx.fbo;

    if (fbo) {
        fbo->bind();
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    material->use();
    material->setMat4("model", ctx.matrices.model);

    if (material->supportsShadows()) {
        material->setMat4(
            "lightSpaceTransform", m_directionalLightSource->getLightSpaceTransform()
        );
        material->setVec3("lightPos", m_directionalLightSource->lightPosition());
        material->setVec3("lightColor", m_directionalLightSource->lightColor());
        material->setVec3("lightDir", m_directionalLightSource->direction());
        material->setVec3("viewPos", ctx.camera->position());

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_directionalLightSource->shadowMapTexture());
        material->setInt("shadowMap", 1);
    }

    if (!renderPass->material) {
        if (ctx.matrices.view.has_value()) {
            material->setMat4("view", ctx.matrices.view.value());
        } else if (ctx.camera) {
            material->setMat4("view", ctx.camera->getView());
        }

        if (ctx.matrices.projection.has_value()) {
            material->setMat4("projection", ctx.matrices.projection.value());
        } else if (ctx.camera) {
            material->setMat4("projection", ctx.camera->getProjection());
        }
    }


    ctx.attributes->bind();
    glDrawArrays(GL_TRIANGLES, 0, n);
}

void Engine::render(GroupRenderContext& ctx, const RenderPass* renderPass) const {
    if (ctx.drawCalls.empty())
        return;

    // Apply override if set
    const Material* material = renderPass->material ? renderPass->material : ctx.material;
    const gl::FBO* fbo = renderPass->fbo ? renderPass->fbo : ctx.fbo;

    if (fbo) {
        fbo->bind();
    }

    material->use();

    if (!renderPass->material) {
        if (ctx.matrices.view.has_value()) {
            material->setMat4("view", ctx.matrices.view.value());
        } else if (ctx.camera) {
            material->setMat4("view", ctx.camera->getView());
        }

        if (ctx.matrices.projection.has_value()) {
            material->setMat4("projection", ctx.matrices.projection.value());
        } else if (ctx.camera) {
            material->setMat4("projection", ctx.camera->getProjection());
        }
    }

    // Render all draw calls with only model matrix and attributes changing
    for (const auto& drawCall : ctx.drawCalls) {
        size_t n = drawCall.attributes->length();
        if (n == 0)
            continue;

        material->setMat4("model", drawCall.model);
        drawCall.attributes->bind();
        glDrawArrays(GL_TRIANGLES, 0, n);
    }
}

void Engine::fireUpdate(float dt) {
    for (auto it = m_updateSubscribers.begin(); it != m_updateSubscribers.end();) {
        if (auto subscriber = it->lock()) {
            subscriber->update(dt);
            ++it;
        } else {
            it = m_updateSubscribers.erase(it);  // clean up expired
        }
    }
}

void Engine::subscribeUpdate(std::shared_ptr<Updateable> updateable) {
    m_updateSubscribers.push_back(updateable);
}

void Engine::beginFrame() {
    m_window->clearScreen();
}

void Engine::endFrame() {
    m_window->swapBuffers();
    glfwPollEvents();
}

void Engine::setDirectionalLightSource(
    std::shared_ptr<engine::Sun> lightSource, uint8_t passPosition
) {
    m_directionalLightSource = lightSource;
    m_renderPasses.erase(
        std::remove_if(
            m_renderPasses.begin(),
            m_renderPasses.end(),
            [](const auto& pass) { return pass->id() == RenderPass::DirectionalShadow; }
        ),
        m_renderPasses.end()
    );

    // TODO once engine settings is implemented, revisit this (do not register the pass)
    registerRenderPass(
        DirectionalShadowPass::create(
            lightSource->shadowMaterial(), lightSource->shadowFBO(), lightSource->resolution()
        ),
        passPosition
    );
}