#include "UIManager.h"

#include <format>

#include <UI/elements/Button.h>
#include <UI/elements/Image.h>
#include <UI/elements/Label.h>
#include <UI/elements/Panel.h>

// clang-format off
#include <glad/glad.h>  // TODO remove?
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
// clang-format on

#include <render/EngineEvents.h>


using namespace ui;


UIManager::UIManager(glm::ivec2 screenSize)
    : m_renderer(RendererType::OpenGL, screenSize),
      m_fboShader("resources/shaders/UI.vert", "resources/shaders/UI.frag", "UI FBO") {
    Renderer::registerCursorFunction([](CursorType type) {
        switch (type) {
            case CursorType::Pointer:
                if (s_currentCursor) {
                    glfwDestroyCursor(s_currentCursor);
                    s_currentCursor = nullptr;
                }
                s_currentCursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
                glfwSetCursor(glfwGetCurrentContext(), s_currentCursor);
                break;
            case CursorType::Default:
                if (s_currentCursor) {
                    glfwDestroyCursor(s_currentCursor);
                    s_currentCursor = nullptr;
                }
                //s_currentCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
                //glfwSetCursor(glfwGetCurrentContext(), s_currentCursor);
                break;
            case CursorType::Type:
                if (s_currentCursor) {
                    glfwDestroyCursor(s_currentCursor);
                    s_currentCursor = nullptr;
                }
                s_currentCursor = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
                glfwSetCursor(glfwGetCurrentContext(), s_currentCursor);
                break;
        }
    });

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

void UIManager::update(float dt) {
    samples++;
    fps += 1.0f / dt;
    if (samples == 60) {
        float fpsCur = fps / samples;
        if (fpsCur > fpsMax)
            fpsMax = fpsCur;
        else if (fpsCur < fpsMin)
            fpsMin = fpsCur;
        m_fpsLabel->text(std::format("FPS: {}, min: {}, max: {}", fpsCur, fpsMin, fpsMax));
        m_renderer.update(dt);
        samples = 0;
        fps = 0.f;
    }
}

void UIManager::render() {
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

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Restore OpenGL state
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}

void UIManager::windowResizeEvent(ResizeEvent* ev) {
    m_renderer.setViewportSize(glm::ivec2{ev->width, ev->height});
}

void UIManager::mouseMoveEvent(::MouseEvent* pEvent) {
    if (glfwGetInputMode(glfwGetCurrentContext(), GLFW_CURSOR) != GLFW_CURSOR_NORMAL)
        return;

    ui::MouseEvent event{
        .button = ui::MouseBtn::None, .action = ui::Action::Move, .pos = {pEvent->x, pEvent->y}
    };
    m_lastMousePos = event.pos;

    m_renderer.mouseEvent(event);
}

void UIManager::mouseButtonEvent(MouseButtonEvent* pEvent) {
    ui::MouseBtn btn;
    switch (pEvent->button) {
        case GLFW_MOUSE_BUTTON_LEFT: btn = ui::MouseBtn::LMB; break;
        case GLFW_MOUSE_BUTTON_RIGHT: btn = ui::MouseBtn::RMB; break;
        case GLFW_MOUSE_BUTTON_MIDDLE: btn = ui::MouseBtn::Wheel; break;
        default: btn = ui::MouseBtn::None; break;
    }
    ui::Action act;
    switch (pEvent->action) {
        case GLFW_PRESS: act = ui::Action::Press; break;
        case GLFW_RELEASE: act = ui::Action::Release; break;
    }
    ui::MouseEvent event{
        .button = btn,
        .action = act,
        .pos = m_lastMousePos,
    };

    m_renderer.mouseEvent(event);
}

void UIManager::createUI() {
    std::shared_ptr<Panel> mainPanel =
        std::make_shared<Panel>(Pos<Rel, Rel>{0.f, 0.f}, Size<Rel, Rel>{1.f, 1.f});

    m_fpsLabel = std::make_shared<Label>(
        "FPS: 0",
        Pos<Rel, Rel>{0.f, 0.f},
        Style<Label>{
            .color = {1.f, 1.f, 1.f, 1.f}, .font = "resources/fonts/ARIAL.TTF", .fontSize = 50
        },
        AnchorPoint::TopLeft,
        AnchorPoint::TopLeft
    );
    mainPanel->addChild(m_fpsLabel);

    std::shared_ptr<Image> cross = std::make_shared<Image>(
        "resources/ui/crosshair.png",
        Pos<Rel, Rel>{0.f, 0.f},
        Size<Abs, Abs>{32, 32},
        Style<Image>{
            .pixelated = true,
        },
        AnchorPoint::Mid
    );
    mainPanel->addChild(cross);

    m_renderer.setRoot(mainPanel);
}