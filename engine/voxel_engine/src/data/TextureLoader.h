#pragma once

#include <string>

#include <core/texture/TextureArray.h>

namespace engine {

    class TextureLoader {
      public:
        TextureLoader(int slot);

        void load(const char* dirPath, gl::texture::ArraySettings settings = gl::texture::ArraySettings::Pixelated());
        void bind() const;

      private:
        gl::texture::TextureArray m_texArray;

        static std::string getTextureName(const std::string& path);
    };

}  // namespace engine
