#pragma once

#include "TextureBase.h"

namespace gl::texture {
    class Texture2D : public TextureBase {
      public:
        Texture2D(UInt unit) : TextureBase(unit, Type::Texture2D) {};

        void create(Settings settings);
        void load(const gl::ImageData& imageData);
        void loadRaw(int w, int h, int ch, gl::ImageFormat format, Data data);

      private:
        int m_width = 0;
        int m_height = 0;
        int m_channels = 0;
    };
}  // namespace gl::texture