#pragma once

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

#include "../Globals.h"

namespace engine {

    class TextureManager {
      public:
        void add(std::string&& name, TexID id) { m_textures.emplace(name, id); }

        TexID texture(std::string name) { return m_textures.at(name); }

        std::string getTextureName(TexID id) {
            for (const auto& [name, texture] : m_textures) {
                if (texture == id) {
                    return name;
                }
            }
            return "";
        }

        static TextureManager& Get() {
            static TextureManager instance;
            return instance;
        }

      private:
        TextureManager() = default;
        std::unordered_map<std::string, TexID> m_textures;
    };
}  // namespace engine