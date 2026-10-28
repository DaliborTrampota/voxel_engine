#pragma once

#include "RenderPass.h"
#include "events/EngineEventSite.h"

#include <vector>

namespace engine {
    class RenderPassRegistry : public EngineEventSite {
      public:
        static RenderPassRegistry& Get();


        void windowResizeEvent(ResizeEvent* pEvent) override;


        /// @brief Appends `pass` after every registered pass.
        template <typename PassT>
        PassT* registerPass(std::unique_ptr<PassT> pass) {
            return static_cast<PassT*>(insert(m_renderPasses.end(), std::move(pass)));
        }

        /// @brief Registers `pass` right before the pass with id `anchor`, which must be registered.
        template <typename PassT>
        PassT* registerPassBefore(RenderPass::ID anchor, std::unique_ptr<PassT> pass) {
            return static_cast<PassT*>(insert(find(anchor), std::move(pass)));
        }

        /// @brief Registers `pass` right after the pass with id `anchor`, which must be registered.
        template <typename PassT>
        PassT* registerPassAfter(RenderPass::ID anchor, std::unique_ptr<PassT> pass) {
            return static_cast<PassT*>(insert(find(anchor) + 1, std::move(pass)));
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

      private:
        using Iterator = std::vector<std::unique_ptr<RenderPass>>::iterator;

        Iterator find(RenderPass::ID id);
        RenderPass* insert(Iterator at, std::unique_ptr<RenderPass> pass);
    };
}  // namespace engine
