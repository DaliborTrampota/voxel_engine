#include "RenderPassRegistry.h"

#include <algorithm>
#include <cassert>

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

RenderPassRegistry::Iterator RenderPassRegistry::find(RenderPass::ID id) {
    auto it = std::find_if(m_renderPasses.begin(), m_renderPasses.end(), [id](const auto& p) {
        return p->id() == id;
    });

    assert(it != m_renderPasses.end() && "anchor pass not registered");
    return it;
}

RenderPass* RenderPassRegistry::insert(Iterator at, std::unique_ptr<RenderPass> pass) {
    assert(
        std::none_of(
            m_renderPasses.begin(),
            m_renderPasses.end(),
            [&](const auto& p) { return p->id() == pass->id(); }
        ) &&
        "pass id already registered"
    );
    return m_renderPasses.insert(at, std::move(pass))->get();
}
