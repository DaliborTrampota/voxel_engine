#pragma once

#include <LWGL/Context.h>

#include "EngineEventSource.h"

namespace engine {
    class Window : public gl::Context,
                   public EngineEventSource {
      public:
        Window(gl::Context* share = nullptr);
        ~Window();

        int init(int w, int h, const char* name) override;

        void mouseLock(bool state) const;
    };
}  // namespace engine