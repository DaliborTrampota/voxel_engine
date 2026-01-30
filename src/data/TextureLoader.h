#pragma once

#include <filesystem>
#include <string>

#include <LWGL/texture/TextureArray.h>

namespace engine {
    namespace fs = std::filesystem;

    class TextureLoader {
      public:
        /// @brief Load textures from a directory into a texture array
        /// @param target Texture array to load the textures into
        /// @param dirPath Path to the directory containing the textures
        /// @param params Settings for the texture array
        /// @note No need to specify the width, height, layer count and format, they will be automatically determined from the images in the directory
        static void loadArray2D(
            gl::TextureArray* target,
            const fs::path& dirPath,
            gl::TextureParams params = gl::TextureParams::Pixelated()
        );

      private:
        static std::string getTextureName(const fs::path& path);
    };

}  // namespace engine
