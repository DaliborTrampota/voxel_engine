#pragma once

#include "RenderPass.h"
#include "events/EngineEventSite.h"

namespace engine {
    class RenderPassRegistry : public EngineEventSite {
      public:
        static RenderPassRegistry& Get();


        void windowResizeEvent(ResizeEvent* pEvent) override;


        /// @brief Registers a custom render pass at the specified position.
        /// @param pass The concrete RenderPass instance.
        /// @param position Where to insert the pass (default: after scene)
        void registerPass(std::unique_ptr<RenderPass> pass, uint8_t position = 0);

        template <typename PassT>
        void deletePass() {
            m_renderPasses.erase(
                std::remove_if(
                    m_renderPasses.begin(),
                    m_renderPasses.end(),
                    [](const auto& pass) { return dynamic_cast<PassT*>(pass.get()) != nullptr; }
                ),
                m_renderPasses.end()
            );
        }

        std::vector<std::unique_ptr<RenderPass>>& passes() { return m_renderPasses; }

        template <typename PassT>
        PassT* getPass() const {
            for (auto& pass : m_renderPasses) {
                if (PassT* concretePass = dynamic_cast<PassT*>(pass.get())) {
                    return concretePass;
                }
            }
            return nullptr;
        }

      protected:
        RenderPassRegistry() { m_renderPasses.reserve(std::numeric_limits<RenderPass::ID>::max()); }

        std::vector<std::unique_ptr<RenderPass>> m_renderPasses;
    };
}  // namespace engine