#include "UIManager.h"

#include <UI/Panel.h>

#include <glad/glad.h>


using namespace ui;


UIManager::UIManager(glm::ivec2 screenSize)
    : m_renderer(RendererType::OpenGL, screenSize),
      m_fboShader("shaders/UI.vert", "shaders/UI.frag", "UI FBO") {
    std::shared_ptr<Panel> mainPanel = std::make_shared<Panel>(
        Pos<Rel, Rel>{0.f, 0.f},
        Size<Rel, Rel>{1.f, 1.f},
        Style<Panel>{.backgroundColor = {1.f, .0f, .0f, 0.0f}, .roundRadius = 0.f}
    );

    std::shared_ptr<Panel> childPanel = std::make_shared<Panel>(
        Pos<Abs, Rel>{0.f, 0.5f},
        Size<Rel, Rel>{0.25f, 0.25f},
        Style<Panel>{.backgroundColor = {0.f, 1.0f, .0f, 1.f}, .roundRadius = 50.f}
    );

    mainPanel->addChild(childPanel);
    m_renderer.setRoot(mainPanel);

    m_fboShader.use();
    m_fboShader.setInt("screenTexture", 0);

    glGenVertexArrays(1, &m_vaoID);
    glGenBuffers(1, &m_vboID);

    glBindVertexArray(m_vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void UIManager::render() {
    m_renderer.update(0.0f);

    float quadVertices[] = {
        // positions   // texCoords
        -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f
    };

    m_fboShader.use();

    // Disable depth test so UI always renders on top
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glBindVertexArray(m_vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // Enable blending for transparency support
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_renderer.textureID());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Restore OpenGL state
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}