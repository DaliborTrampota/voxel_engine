#include "Engine.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <core/render/Material.h>

#include "level/Chunk.h"
#include "level/World.h"
#include "scene/Camera.h"
#include "scene/Updateable.h"
#include "render/Renderable.h"
#include "render/RenderContext.h"


using namespace engine;

Engine::Engine(std::unique_ptr<gl::Window> window) : m_window(std::move(window)) {}

void Engine::submitRender(RenderContext&& ctx, bool immediate) {
    if (!immediate) {
        m_renderQueue.push_back(std::move(ctx));
        return;
    }
    
    size_t n = ctx.attributes->length();
    if(n == 0)
        return;

    ctx.material->use();
    ctx.material->setMat4("model", ctx.modelMatrix);
    // if(ctx.viewMatrixOverride)
    //     ctx.material->setMat4("view", ctx.viewMatrixOverride);

    ctx.attributes->bind();
    glDrawArrays(GL_TRIANGLES, 0, n);
}

void Engine::flush() {
    for (auto& ctx : m_renderQueue) {
        render(ctx);
    }
    m_renderQueue.clear();
}

void Engine::gameloop() {
    m_window->setRenderFlags();

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    while (!m_window->shouldClose()) {
        float currentFrame = m_window->time();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        m_window->beginFrame();

        beforeRender();
        render(deltaTime);
        flush();
        afterRender();

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
            printf("OpenGL error: %d\n", err);

        m_window->endFrame();
    }
}

void Engine::render(RenderContext& ctx) const {
    size_t n = ctx.attributes->length();
    if(n == 0)
        return;

    ctx.material->use();
    ctx.material->setMat4("model", ctx.modelMatrix);
    // ctx.material->setMat4("view", ctx.camera->getView());
    // ctx.material->setMat4("projection", ctx.camera->getProjection());
    // if(ctx.viewMatrixOverride)
    //     ctx.material->setMat4("view", ctx.viewMatrixOverride);

    ctx.attributes->bind();
    glDrawArrays(GL_TRIANGLES, 0, n);
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

// void Engine::subscribeInputSystem(engine::InputSystem* inputSystem) {
//     m_window->graphicsAPI()->subscribe(inputSystem);
// }
