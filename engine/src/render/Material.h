#pragma once

#include <LWGL/render/ShaderProgram.h>

namespace engine {

    class Material : public gl::ShaderProgram {
      public:
        using gl::ShaderProgram::ShaderProgram;

        void setShadowSupport(bool supportsShadows) { m_supportsShadows = supportsShadows; }
        bool supportsShadows() const { return m_supportsShadows; }

      private:
        bool m_supportsShadows = false;
    };
}  // namespace engine