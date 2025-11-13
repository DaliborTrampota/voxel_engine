#include "UIManager.h"

#include <format>

#include <UI/elements/Button.h>
#include <UI/elements/Image.h>
#include <UI/elements/Label.h>
#include <UI/elements/Panel.h>

#include <GLFW/glfw3.h>
#include <glad/glad.h>  // TODO remove?

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

    // std::shared_ptr<Panel> childPanel = std::make_shared<Panel>(
    //     Pos<Abs, Rel>{0.f, 0.0f},
    //     Size<Rel, Rel>{0.5f, 0.5f},
    //     Style<Panel>{
    //         .backgroundColor = {0.f, 1.0f, .0f, 0.8f},
    //         .roundRadius = 50,
    //         .borderThickness = 30,
    //         .borderColor = {1.f, 0.f, 1.f}
    //     }
    // );

    // std::shared_ptr<Panel> anchorTest = std::make_shared<Panel>(
    //     Pos<Abs, Abs>{100, 50},
    //     Size<Rel, Rel>{0.25f, 0.1f},
    //     Style<Panel>{
    //         .backgroundColor = {.05f, 0.2f, .1f, 0.8f},
    //         .roundRadius = 0,
    //         .borderThickness = 10,
    //         .borderColor = {1.f, 1.f, 1.f}
    //     },
    //     AnchorPoint::BottomRight
    // );
    // childPanel->addChild(anchorTest);

    // std::shared_ptr<Panel> childPanel2 = std::make_shared<Panel>(
    //     Pos<Rel, Rel>{0.5f, 0.5f},
    //     Size<Rel, Rel>{0.5f, 0.5f},
    //     Style<Panel>{
    //         .backgroundColor = {.05f, 0.2f, .1f, 0.8f},
    //         .roundRadius = 0,
    //         .borderThickness = 10,
    //         .borderColor = {1.f, 1.f, 1.f}
    //     }
    // );

    // auto makeAnchorPanel = [&](AnchorPoint pt) {
    //     std::shared_ptr<Panel> anchorPanel = std::make_shared<Panel>(
    //         Pos<Abs, Abs>{0, 0},
    //         Size<Abs, Abs>{50, 50},
    //         //Size<Rel, Rel>{0.5f, 0.5f},
    //         Style<Panel>{
    //             .backgroundColor = {1.f, 0.f, 1.f, 0.8f},
    //             .roundRadius = 0,
    //             .borderThickness = 10,
    //             .borderColor = {1.f, 1.f, 1.f}
    //         },
    //         pt
    //     );

    //     childPanel2->addChild(anchorPanel);
    // };

    // makeAnchorPanel(AnchorPoint::TopLeft);
    // makeAnchorPanel(AnchorPoint::TopRight);
    // makeAnchorPanel(AnchorPoint::BottomLeft);
    // makeAnchorPanel(AnchorPoint::BottomRight);
    // makeAnchorPanel(AnchorPoint::Mid);
    // makeAnchorPanel(AnchorPoint::Top);
    // makeAnchorPanel(AnchorPoint::Bottom);
    // makeAnchorPanel(AnchorPoint::Left);
    // makeAnchorPanel(AnchorPoint::Right);

    // std::shared_ptr<Image> image = std::make_shared<Image>(
    //     "resources/ui/test.png",
    //     Pos<Abs, Abs>{100, 100},
    //     Size<Abs, Abs>{500, 500},
    //     Style<Image>{.opacity = 0.8f, .pixelated = true},
    //     AnchorPoint::BottomLeft
    //     // Style<Image>{
    //     //     .backgroundColor = {1.f, 1.f, 1.f, 0.8f},
    //     // },
    // );

    // using BtnStyle = Style<Button>;

    // BtnStyle::State normal{.background{1.f, 0.f, 0.f, 0.8f}, .border{1.f, 1.f, 1.f}};
    // BtnStyle::State hover{.background{1.f, 0.f, 1.f, 0.8f}, .border{0.f, 1.f, 1.f}};
    // BtnStyle::State pressed{.background{0.f, 1.f, 1.f, 0.8f}, .border{0.f, 1.f, 0.f}};


    // auto btnStyle = Style<Button>{
    //     .normal = normal,
    //     .hovered = hover,
    //     .pressed = pressed,
    //     .roundRadius = 20,
    //     .borderThickness = 2,
    // };

    // std::shared_ptr<Button> button = std::make_shared<Button>(
    //     Pos<Abs, Abs>{600, 100}, Size<Abs, Abs>{200, 100}, btnStyle, AnchorPoint::BottomLeft
    // );

    // std::shared_ptr<Label> text = std::make_shared<Label>(
    //     "Hello world!",
    //     Pos<Rel, Rel>{0.f, 0.f},
    //     Style<Label>{
    //         .color = {1.f, 0.2f, 0.f, 1.f}, .font = "resources/fonts/ARIAL.TTF", .fontSize = 100
    //     },
    //     AnchorPoint::Mid,
    //     AnchorPoint::Top
    // );


    // mainPanel->addChild(button);
    // mainPanel->addChild(image);
    // mainPanel->addChild(text);

    // mainPanel->addChild(childPanel);
    // mainPanel->addChild(childPanel2);
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