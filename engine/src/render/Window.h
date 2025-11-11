#pragma once

#include <LWGL/Context.h>

namespace engine {
    class Window : public gl::Context {
      public:
        Window(gl::Context* share = nullptr);
        ~Window();

        int init(int w, int h, const char* name) override;

        void mouseLock(bool state) const;
    };
}  // namespace engine