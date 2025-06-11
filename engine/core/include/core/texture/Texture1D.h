#pragma once

#include "TextureBase.h"

namespace gl::texture {
    class Texture1D : public TextureBase {
      public:
        Texture1D(UInt unit) : TextureBase(unit, Type::Texture1D) {};

        void create(Settings settings);
        void load(const gl::ImageData& imageData);
        void loadRaw(int w, int ch, gl::ImageFormat format, Data data);

      private:
        int m_width = 0;
        int m_channels = 0;
    };
}  // namespace gl::texture