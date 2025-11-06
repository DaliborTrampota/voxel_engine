#pragma once

#include <string>

#include <LWGL/texture/TextureArray.h>

namespace engine {

    class TextureLoader {
      public:
        TextureLoader(int slot);

        void load(const char* dirPath, gl::ArraySettings settings = gl::ArraySettings::Pixelated());
        void bind() const;

      private:
        gl::TextureArray m_texArray;

        static std::string getTextureName(const std::string& path);
    };

}  // namespace engine
