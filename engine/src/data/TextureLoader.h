#pragma once

#include <filesystem>
#include <string>


#include <LWGL/texture/TextureArray.h>

namespace engine {
    namespace fs = std::filesystem;

    class TextureLoader {
      public:
        explicit TextureLoader(int slot);

        /// @brief Load textures from a directory
        /// @param dirPath Path to the directory containing the textures
        /// @param settings Settings for the texture array
        /// @note No need to specify the width, height, layer count and format, they will be automatically determined from the images in the directory
        void load(
            const fs::path& dirPath, gl::ArraySettings settings = gl::ArraySettings::Pixelated()
        );
        void bind() const;

      private:
        gl::TextureArray m_texArray;

        static std::string getTextureName(const fs::path& path);
    };

}  // namespace engine
