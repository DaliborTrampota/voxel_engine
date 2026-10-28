#include "RenderPassRegistry.h"

#include "events/EngineEvents.h"

using namespace engine;

RenderPassRegistry& RenderPassRegistry::Get() {
    static RenderPassRegistry s_instance;
    return s_instance;
}


void RenderPassRegistry::windowResizeEvent(ResizeEvent* pEvent) {
    for (auto& pass : m_renderPasses) {
        pass->resize(glm::ivec2(pEvent->width, pEvent->height));
    }
}

void RenderPassRegistry::registerPass(std::unique_ptr<RenderPass> pass, uint8_t position) {
    position = glm::min(position, static_cast<uint8_t>(m_renderPasses.size()));
    m_renderPasses.insert(m_renderPasses.begin() + position, std::move(pass));

void RenderPassRegistry::registerPassAfter(RenderPass::ID anchor, std::unique_ptr<RenderPass> pass) {
    auto it = std::find_if(m_renderPasses.begin(), m_renderPasses.end(), [anchor](const auto& p) {
        return p->id() == anchor;
    });
    m_renderPasses.insert(it == m_renderPasses.end() ? it : it + 1, std::move(pass));
}
