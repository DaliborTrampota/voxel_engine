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
    m_cubeMap.upload(gl::CubeFace::Top, gl::ImageData(settings.top.c_str(), gl::ColorSpace::sRGB));
    m_cubeMap.upload(
        gl::CubeFace::Bottom, gl::ImageData(settings.bottom.c_str(), gl::ColorSpace::sRGB)
    );
    m_cubeMap.upload(
        gl::CubeFace::Front, gl::ImageData(settings.front.c_str(), gl::ColorSpace::sRGB)
    );
    m_cubeMap.upload(
        gl::CubeFace::Back, gl::ImageData(settings.back.c_str(), gl::ColorSpace::sRGB)
    );
    m_cubeMap.upload(
        gl::CubeFace::Left, gl::ImageData(settings.left.c_str(), gl::ColorSpace::sRGB)
    );
    m_cubeMap.upload(
        gl::CubeFace::Right, gl::ImageData(settings.right.c_str(), gl::ColorSpace::sRGB)
    );

    m_material.use();
    m_material.setTexture(0, &m_cubeMap, "skybox");
}

void Skybox::render(Engine& engine, const Camera* camera, int pass) {
    // if (pass != 0)
    //     return;  // Only render in the first pass

    RenderContext ctx;
    ctx.material = &m_material;
    ctx.attributes = &m_buffer;
    ctx.camera = camera;
    ctx.passMask = RenderPass::Scene;
    ctx.matrices.projection = camera->getProjection();
    ctx.matrices.view = glm::mat4(glm::mat3(camera->getView()));
    ctx.matrices.model = glm::mat4(1.0f);
    engine.submitRender(std::move(ctx));

    // TODO workaround in shader for depth testing with skybox
    // glDepthFunc(GL_LEQUAL);
    // glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_buffer.length()));
    // glDepthFunc(GL_LESS);
}
