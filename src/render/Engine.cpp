#include "Engine.h"

#include <algorithm>
#include <cstdint>
#include <format>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include <LWGL/buffer/Attributes.h>
#include <LWGL/buffer/FBO.h>
#include <LWGL/indirect/IndirectBuffer.h>
#include <LWGL/indirect/VertexPool.h>

#include "RenderContext.h"
#include "RenderPass.h"
#include "data/RegistryManager.h"
#include "input/InputSystem.h"
#include "level/Chunk.h"
#include "level/World.h"
#include "render/Material.h"
#include "render/Renderable.h"
#include "scene/Camera.h"
#include "scene/DirectionalLight.h"
#include "scene/Tickable.h"
#include "scene/Updateable.h"
#include "utility/UtilityShaders.h"

#include "concrete/ClusterBuildPass.h"
#include "concrete/CompositePass.h"
#include "concrete/DirectionalShadowPass.h"
#include "concrete/PointLightShadowPass.h"
#include "concrete/ScenePass.h"
#include "concrete/TransparentPass.h"

#include "utility/GPUProfiler.h"

static engine::GPUProfiler gpuProfiler;


using namespace engine;

std::string RenderStats::toString() const {
    return std::format(
        "Draw Calls: {}, Vertices: {}, Triangles: {}, Frame Time: {:.2f} ms",
        drawCalls,
        vertices,
        triangles,
        frameTime * 1000.0
    );
}


Engine::Engine(std::unique_ptr<Window> window)
    : m_window(std::move(window)),
      m_inputSystem(std::make_unique<InputSystem>()),
      m_passRegistry(&RenderPassRegistry::Get()) {
    m_window->subscribe(m_passRegistry);
    Material::setGlobalConstant("CascadeCount", 4);

    glm::ivec2 resolution = m_window->windowSize();
    m_passRegistry->registerPass(std::make_unique<ScenePass>(resolution), 0);
    m_passRegistry->registerPass(std::make_unique<TransparentPass>(resolution), 1);
    m_passRegistry->registerPass(std::make_unique<CompositePass>(resolution), 2);

    RegistryManager::Blocks().add(Block::air(), "air");
    RegistryManager::Blocks().add(Block::multiblock(), "multiblock");
    // RegistryManager::Blocks().add(Block(2, Layers::Any, nullptr), "reserved_block_2");
    // RegistryManager::Blocks().add(Block(3, Layers::Any, nullptr), "reserved_block_3");
    // RegistryManager::Blocks().add(Block(4, Layers::Any, nullptr), "reserved_block_4");
    // RegistryManager::Blocks().add(Block(5, Layers::Any, nullptr), "reserved_block_5");
    // RegistryManager::Blocks().add(Block(6, Layers::Any, nullptr), "reserved_block_6");
    // RegistryManager::Blocks().add(Block(7, Layers::Any, nullptr), "reserved_block_7");
    // RegistryManager::Blocks().add(Block(8, Layers::Any, nullptr), "reserved_block_8");
    gpuProfiler.init();
}


void Engine::submitRender(RenderContext&& ctx, bool immediate) {
    if (!immediate) {
        m_renderQueue.push_back(std::move(ctx));
        return;
    }

    for (const auto& pass : m_passRegistry->passes()) {
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

    for (const auto& pass : m_passRegistry->passes()) {
        if ((ctx.passMask & pass->id()) != 0) {
            for (uint8_t subPass = 0; subPass < pass->passes(); subPass++) {
                pass->beforeRender(*this, subPass);
                render(ctx, pass.get());
                pass->afterRender(*this, subPass);
            }
        }
    }
}

void Engine::submitRender(IndirectRenderContext&& ctx, bool immediate) {
    if (!immediate) {
        m_renderQueue.push_back(std::move(ctx));
        return;
    }

    for (const auto& pass : m_passRegistry->passes()) {
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
    gpuProfiler.beginFrame();
    //printf("Flush %zu\n", m_renderQueue.size());
    for (const auto& pass : m_passRegistry->passes()) {
        GPU_SCOPE(&gpuProfiler, std::format("Pass {}", pass->id()));
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
    gpuProfiler.endFrame();
    m_renderQueue.clear();
}


void Engine::gameloop() {
    //TODO
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
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

    if (material->supportsShadows() && m_activeDirectionalLightSource) {
        assert(ctx.camera && "RenderContext Camera is required when material supports shadows");
        // material->setVec3("lightPos", m_directionalLightSource->lightPosition());
        material->setVec3("lightColor", m_activeDirectionalLightSource->lightColor());
        material->setVec3("lightDir", -m_activeDirectionalLightSource->direction());
        material->setVec3("viewPos", ctx.camera->position());


        const auto& cascadeSplits =
            m_passRegistry->getPass<DirectionalShadowPass>()->cascadeSplits();
        for (size_t i = 0; i < cascadeSplits.size(); i++) {
            material->setFloat(
                std::format("cascadePlaneDistances[{}]", i), cascadeSplits[i].farPlane
            );
        }
    }

    if (material->supportsMVP()) {
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
    material->bindTextures();
    glDrawArrays(GL_TRIANGLES, 0, n);

    m_renderStats.drawCalls++;
    m_renderStats.vertices += n;
    m_renderStats.triangles += n / 3;
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

        m_renderStats.drawCalls++;
        m_renderStats.vertices += n;
        m_renderStats.triangles += n / 3;
    }
}

void Engine::render(IndirectRenderContext& ctx, const RenderPass* renderPass) const {
    size_t n = ctx.batch->drawCount();
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

    if (material->supportsShadows() && m_activeDirectionalLightSource) {
        assert(
            ctx.camera && "IndirectRenderContext Camera is required when material supports shadows"
        );
        // material->setVec3("lightPos", m_directionalLightSource->lightPosition());
        material->setVec3("lightColor", m_activeDirectionalLightSource->lightColor());
        material->setVec3("lightDir", -m_activeDirectionalLightSource->direction());
        material->setVec3("viewPos", ctx.camera->position());

        const auto& cascadeSplits =
            m_passRegistry->getPass<DirectionalShadowPass>()->cascadeSplits();
        for (size_t i = 0; i < cascadeSplits.size(); i++) {
            material->setFloat(
                std::format("cascadePlaneDistances[{}]", i), cascadeSplits[i].farPlane
            );
        }
    }

    if (material->supportsMVP()) {
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


    // if (ctx.depthFunc == DepthFunc::LessEqual)
    //     glDepthFunc(GL_LEQUAL);

    ctx.pool->bind();
    material->bindTextures();
    ctx.batch->draw();

    m_renderStats.drawCalls++;
    m_renderStats.vertices += ctx.batch->verticesCount();
    m_renderStats.triangles += ctx.batch->verticesCount() / 3;
}

void Engine::fireUpdate(float dt) {
    m_tickAccumulator += dt;
    bool tick = m_tickAccumulator >= TickRate;

    for (auto it = m_updateSubscribers.begin(); it != m_updateSubscribers.end();) {
        if (auto subscriber = it->lock()) {
            subscriber->update(dt);
            ++it;
        } else {
            it = m_updateSubscribers.erase(it);  // clean up expired
        }
    }
    if (tick) {
        m_tickAccumulator -= TickRate;
        for (auto it = m_tickSubscribers.begin(); it != m_tickSubscribers.end();) {
            if (auto subscriber = it->lock()) {
                subscriber->tick(TickRate);
                ++it;
            } else {
                it = m_tickSubscribers.erase(it);  // clean up expired
            }
        }
    }
}

void Engine::subscribeUpdate(std::shared_ptr<Updateable> updateable) {
    m_updateSubscribers.push_back(updateable);
}

void Engine::unsubscribeUpdate(Updateable* updateable) {
    for (auto it = m_updateSubscribers.begin(); it != m_updateSubscribers.end();) {
        if (it->lock().get() == updateable) {
            it = m_updateSubscribers.erase(it);
            break;
        } else {
            ++it;
        }
    }
}

void Engine::subscribeTick(std::shared_ptr<Tickable> tickable) {
    m_tickSubscribers.push_back(tickable);
}

void Engine::unsubscribeTick(Tickable* tickable) {
    for (auto it = m_tickSubscribers.begin(); it != m_tickSubscribers.end();) {
        if (it->lock().get() == tickable) {
            it = m_tickSubscribers.erase(it);
            break;
        } else {
            ++it;
        }
    }
}

void Engine::beginFrame() {
    m_renderStats._frameStart = m_window->timeDouble();
    m_window->clearScreen();
    m_inputSystem->beginFrame();
    glfwPollEvents();
}

void Engine::endFrame() {
    m_window->swapBuffers();
    m_renderStats.frameTime = m_window->timeDouble() - m_renderStats._frameStart;

    // printf("Render stats: %s\n", m_renderStats.toString().c_str());
    m_renderStats.reset();
}

gl::TextureArray& Engine::setDirectionalLightSource(
    std::shared_ptr<engine::DirectionalLight> lightSource, Camera* camera
) {
    if (m_activeDirectionalLightSource) {
        m_passRegistry->deletePass<DirectionalShadowPass>();
    }

    m_activeDirectionalLightSource = lightSource.get();

    // TODO once engine settings is implemented, revisit this (do not register the pass)
    m_passRegistry->registerPass(
        std::make_unique<DirectionalShadowPass>(
            m_window->windowSize(), camera, m_activeDirectionalLightSource, glm::ivec2{4096, 4096}
        ),
        0
    );

    auto& directionalShadowMaps =
        m_passRegistry->getPass<DirectionalShadowPass>()->cascadeShadowMaps();
    m_passRegistry->getPass<TransparentPass>()->setDirectionalShadowMaps(directionalShadowMaps);
    return directionalShadowMaps;
}

// TODO track active camera in engine
gl::CubeMapArray& Engine::setPointLightSource(PointLightManager* pointLightManager, Camera* camera) {
    if (m_activePointLightManager) {
        m_passRegistry->deletePass<ClusterBuildPass>();
        m_passRegistry->deletePass<PointLightShadowPass>();
    }

    m_activePointLightManager = pointLightManager;
    m_passRegistry->registerPass(
        std::make_unique<ClusterBuildPass>(
            m_window->windowSize(), camera, m_activePointLightManager
        ),
        0
    );

    m_passRegistry->registerPass(
        std::make_unique<PointLightShadowPass>(
            m_window->windowSize(), m_activePointLightManager, glm::ivec2(1024, 1024)
        ),
        0
    );

    return m_passRegistry->getPass<PointLightShadowPass>()->shadowMaps();
}