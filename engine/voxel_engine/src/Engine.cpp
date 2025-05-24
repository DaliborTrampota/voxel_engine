#include "Engine.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <core/gl/GraphicsAPI.h>

#include <Camera.h>
#include <core/gl/ShaderPipeline.h>
#include <level/Chunk.h>
#include <level/World.h>

#include "InputSystem.h"
#include "Updateable.h"


using namespace engine;

Engine::Engine(std::unique_ptr<gl::Window> window) : m_window(std::move(window)) {
    InputSystem::s_instance = new InputSystem();
    m_window->graphicsAPI()->subscribe(InputSystem::s_instance);
}

void Engine::render(gl::ShaderPipeline* pipeline, Chunk* chunk) {
    size_t verts = chunk->m_vertexData.length();
    if (verts == 0 || !chunk->m_generated)
        return;

    chunk->m_vertexData.bind();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(chunk->m_coords * Chunk::Dims));
    pipeline->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, verts);
}

void Engine::render(gl::ShaderPipeline* pipeline, Camera* cam, std::shared_ptr<World> world) {
    pipeline->setViewMatrix(cam->getView());
    for (auto& pos : world->m_loadedChunks) {
        render(pipeline, world->m_chunks[pos]);
    }
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
        afterRender();

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
            printf("OpenGL error: %d\n", err);

        m_window->endFrame();
    }
}

void Engine::fireUpdate(float dt) {
    for (auto it = m_updateSubscribers.begin(); it != m_updateSubscribers.end();) {
        if (auto subscriber = it->lock()) {
            subscriber->update(dt);
            ++it;
        } else {
            it = m_updateSubscribers.erase(it); // clean up expired
        }
    }
}

void Engine::subscribeUpdate(std::shared_ptr<Updateable> updateable) {
    m_updateSubscribers.push_back(updateable);
}

// void Engine::subscribeInputSystem(engine::InputSystem* inputSystem) {
//     m_window->graphicsAPI()->subscribe(inputSystem);
// }
