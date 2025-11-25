#include "Engine.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <LWGL/buffer/FBO.h>

#include "level/Chunk.h"
#include "level/World.h"
#include "render/Material.h"
#include "render/Renderable.h"
#include "scene/Camera.h"
#include "scene/Sun.h"
#include "scene/Updateable.h"

using namespace engine;

Engine::Engine(std::unique_ptr<Window> window) : m_window(std::move(window)) {
    registerRenderPass({RenderPass::DirectionalShadow});
    // registerRenderPass({RenderPass::OmniShadow});
    registerRenderPass({RenderPass::Scene});
}

void Engine::submitRender(RenderContext&& ctx, bool immediate) {
    if (!immediate) {
        m_renderQueue.push_back(std::move(ctx));
        return;
    }

    render(ctx, RenderPass::Scene);
}

void Engine::submitRender(GroupRenderContext&& ctx, bool immediate) {
    if (!immediate) {
        m_renderQueue.push_back(std::move(ctx));
        return;
    }

    render(ctx, RenderPass::Scene);
}

void Engine::flush() {
    for (const auto& pass : m_renderPasses) {
        setRenderOverride(pass.materialOverride, pass.fboOverride);

        // Clear the FBO if it's bound
        if (pass.fboOverride) {
            pass.fboOverride->bind();

            pass.fboOverride->clearDepth(1.0f);
        }

        if (pass.viewportSize.has_value()) {
            glm::ivec2 res = pass.viewportSize.value();
            glViewport(0, 0, res.x, res.y);
        }
        for (auto& ctxVariant : m_renderQueue) {
            std::visit(
                [this, &pass](auto& ctx) {
                    if ((ctx.passMask & pass.id) == 0)
                        return;
                    this->render(ctx, pass.id);
                },
                ctxVariant
            );
        }
        if (pass.viewportSize.has_value()) {
            glm::ivec2 res = m_window->windowSize();
            glViewport(0, 0, res.x, res.y);
        }
    }

    clearRenderOverride();
    m_renderQueue.clear();
}

void Engine::registerRenderPass(const RenderPass::Config& config) {
    int idx = static_cast<int>(glm::log2(static_cast<float>(config.id)));
    if (idx >= m_renderPasses.size()) {
        m_renderPasses.push_back(config);
    } else {
        m_renderPasses[idx] = config;
    }
}

void Engine::setRenderPassOrder(const std::vector<RenderPass::ID>& order) {
    if (order.size() != m_renderPasses.size()) {
        throw std::runtime_error("Render pass order size mismatch");
    }
    std::vector<RenderPass::Config> newOrder;
    for (const auto& id : order) {
        auto it = std::find_if(
            m_renderPasses.begin(), m_renderPasses.end(), [id](const RenderPass::Config& config) {
                return config.id == id;
            }
        );
        if (it == m_renderPasses.end()) {
            throw std::runtime_error("Render pass not found: " + std::to_string(id));
        }
        newOrder.push_back(*it);
    }
    m_renderPasses = newOrder;
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

void Engine::render(RenderContext& ctx, RenderPass::ID renderPass) const {
    size_t n = ctx.attributes->length();
    if (n == 0)
        return;

    // Apply override if set
    const Material* material = m_renderOverride.material ? m_renderOverride.material : ctx.material;
    const gl::FBO* fbo = m_renderOverride.fbo ? m_renderOverride.fbo : ctx.fbo;

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
        material->setVec3("viewPos", ctx.camera->position());
        material->setInt("shadowMap", m_directionalLightSource->shadowMapTexture());
    }

    if (!m_renderOverride.material) {
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

void Engine::render(GroupRenderContext& ctx, RenderPass::ID renderPass) const {
    if (ctx.drawCalls.empty())
        return;

    // Apply override if set
    const Material* material = m_renderOverride.material ? m_renderOverride.material : ctx.material;
    const gl::FBO* fbo = m_renderOverride.fbo ? m_renderOverride.fbo : ctx.fbo;

    if (fbo) {
        fbo->bind();
    }

    material->use();

    if (!m_renderOverride.material) {
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
// void Engine::subscribeInputSystem(engine::InputSystem* inputSystem) {
//     m_window->graphicsAPI()->subscribe(inputSystem);
// }
