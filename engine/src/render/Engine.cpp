#include "Engine.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <LWGL/buffer/FBO.h>
#include <LWGL/render/Material.h>

#include "level/Chunk.h"
#include "level/World.h"
#include "render/RenderContext.h"
#include "render/Renderable.h"
#include "scene/Camera.h"
#include "scene/Updateable.h"


using namespace engine;

Engine::Engine(std::unique_ptr<Window> window) : m_window(std::move(window)) {}

void Engine::submitRender(RenderContext&& ctx, bool immediate) {
    if (!immediate) {
        m_renderQueue.push_back(std::move(ctx));
        return;
    }

    render(ctx);
}

void Engine::submitRender(GroupRenderContext&& ctx, bool immediate) {
    if (!immediate) {
        m_renderQueue.push_back(std::move(ctx));
        return;
    }

    render(ctx);
}

void Engine::flush() {
    for (auto& ctxVariant : m_renderQueue) {
        std::visit(
            [this, pass](auto& ctx) {
                this->render(ctx);
            },
            ctxVariant
        );
    }
    m_renderQueue.clear();
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

void Engine::render(RenderContext& ctx) const {
    size_t n = ctx.attributes->length();
    if (n == 0)
        return;

    // Apply override if set
    const gl::Material* material =
        m_renderOverride.material ? m_renderOverride.material : ctx.material;
    const gl::FBO* fbo = m_renderOverride.fbo ? m_renderOverride.fbo : ctx.fbo;

    if (fbo) {
        fbo->bind();
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    material->use();
    material->setMat4("model", ctx.matrices.model);

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

    ctx.attributes->bind();
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(n));
}

void Engine::render(GroupRenderContext& ctx) const {
    if (ctx.drawCalls.empty())
        return;

    // Apply override if set
    const gl::Material* material =
        m_renderOverride.material ? m_renderOverride.material : ctx.material;
    const gl::FBO* fbo = m_renderOverride.fbo ? m_renderOverride.fbo : ctx.fbo;

    if (fbo) {
        fbo->bind();
    }

    material->use();

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

    // Render all draw calls with only model matrix and attributes changing
    for (const auto& drawCall : ctx.drawCalls) {
        size_t n = drawCall.attributes->length();
        if (n == 0)
            continue;

        material->setMat4("model", drawCall.model);
        drawCall.attributes->bind();
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(n));
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
