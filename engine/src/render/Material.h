#pragma once

#include <LWGL/render/ShaderProgram.h>

namespace engine {

    class Material : public gl::ShaderProgram {
      public:
        using gl::ShaderProgram::ShaderProgram;

        void setShadowSupport(bool supportsShadows) { m_supportsShadows = supportsShadows; }
        bool supportsShadows() const { return m_supportsShadows; }

        void mvpSupport(bool supportsMVP) { m_supportsMVP = supportsMVP; }
        bool supportsMVP() const { return m_supportsMVP; }

      private:
        bool m_supportsShadows = false;
        bool m_supportsMVP = true;
    };
}  // namespace engine