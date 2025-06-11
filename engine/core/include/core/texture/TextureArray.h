#pragma once

#include "TextureBase.h"

namespace gl::texture {

    class TextureArray : public TextureBase {
      public:
        TextureArray(UInt unit) : TextureBase(unit, Type::TextureArray) {}

        void create(ArraySettings settings);
        int load(const gl::ImageData& imageData, int layer = -1);
        int loadRaw(Data data, int layer = -1);

      private:
        int m_width = 0;
        int m_height = 0;
        int m_channels = 0;
        gl::ImageFormat m_format = gl::ImageFormat::Unknown;
        int m_layer = 0;
        unsigned int m_maxLayers = 0;
    };
}  // namespace gl::texture