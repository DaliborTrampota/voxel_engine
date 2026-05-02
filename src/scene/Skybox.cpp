#include "Skybox.h"

#include <LWGL/texture/ImageData.h>
#include <LWGL/texture/TextureBase.h>

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

Skybox::Skybox(const Settings& settings)
    : m_material(
          "resources/shaders/SkyboxVert.glsl", "resources/shaders/SkyboxFrag.glsl", "Skybox"
      ) {
    m_buffer.create({
        .location = 0,
        .type = gl::VertexAttribute::Type::Float,
        .size = 3,
        .offset = 0,
    });
    m_buffer.reserve(size);
    for (int i = 0; i < size; i++) {
        m_buffer.add(skyboxVertices[i]);
    }

    m_cubeMap.create(gl::TextureParams::Cubemap());
    load(settings);
}

void Skybox::load(const Settings& settings) {
    m_cubeMap.upload(gl::CubeFace::Top, settings.top.c_str());
    m_cubeMap.upload(gl::CubeFace::Bottom, settings.bottom.c_str());
    m_cubeMap.upload(gl::CubeFace::Front, settings.front.c_str());
    m_cubeMap.upload(gl::CubeFace::Back, settings.back.c_str());
    m_cubeMap.upload(gl::CubeFace::Left, settings.left.c_str());
    m_cubeMap.upload(gl::CubeFace::Right, settings.right.c_str());

    m_material.use();
    m_material.setTexture(0, &m_cubeMap, "skybox");
}

void Skybox::render(Engine& engine, const Camera* camera, int pass) {
    RenderContext ctx;
    ctx.material = &m_material;
    ctx.attributes = &m_buffer;
    ctx.matrices.projection = camera->getProjection();
    ctx.matrices.view = glm::mat4(glm::mat3(camera->getView()));
    ctx.matrices.model = glm::mat4(1.0f);
    ctx.depthFunc = DepthFunc::LessEqual;
    engine.submitRender(std::move(ctx));
}
