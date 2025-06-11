#pragma once

#include "TextureBase.h"

namespace gl::texture {

    class CubeMap : public TextureBase {
      public:
        CubeMap(UInt unit) : TextureBase(unit, Type::CubeMap) {};

        void create(Settings setting);
        void loadFace(CubeFace face, const gl::ImageData& data);
        void loadFaceRaw(CubeFace face, int w, int ch, gl::ImageFormat format, Data data);

      private:
        int m_width = 0;
        int m_channels = 0;
    };
}  // namespace gl::texture