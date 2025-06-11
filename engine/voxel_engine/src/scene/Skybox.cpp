#include "Skybox.h"

#include "render/Engine.h"
#include "scene/Camera.h"

using namespace engine;

namespace {
    float skyboxVertices[] = {
        // positions
        -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
        1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

        -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
        -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

        1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
        1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f
    };
    constexpr int size = sizeof(skyboxVertices) / sizeof(float);
}  // namespace

Skybox::Skybox(unsigned int unit)
    : gl::texture::CubeMap(unit),
      m_material(
          "resources/shaders/SkyboxVert.glsl", "resources/shaders/SkyboxFrag.glsl", "Skybox"
      ) {
    m_attributes.create();
    m_attributes.reserve(size / 3);  // 3 floats per vertex
    for (int i = 0; i < size; i += 3) {
        m_attributes.add(
            glm::vec3(skyboxVertices[i], skyboxVertices[i + 1], skyboxVertices[i + 2])
        );
    }
}

void Skybox::render(Engine& engine, const Camera* camera, int pass) {
    if (pass != 0)
        return;  // Only render in the first pass

    RenderContext ctx;
    ctx.material = &m_material;
    ctx.attributes = &m_attributes;

    glDepthFunc(GL_LEQUAL);
    m_material.use();
    m_attributes.bind();
    bind();

    glm::mat4 view = glm::mat4(glm::mat3(camera->getView()));

    m_material.setMat4("projection", camera->getProjection());
    m_material.setMat4("view", view);

    glDrawArrays(GL_TRIANGLES, 0, m_attributes.length());

    // engine.submitRender(std::move(ctx));
    glDepthFunc(GL_LESS);
}
