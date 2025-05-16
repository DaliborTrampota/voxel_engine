#pragma once

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

namespace engine {

	class TextureManager
	{
	public:

		void add(std::string &&name, unsigned int id) {
			m_textures.emplace(name, id);
		}

		unsigned int texture(std::string name) {
			return m_textures.at(name);
		}

        static TextureManager& Get() {
            static TextureManager instance;
            return instance;
        }

	private:
        TextureManager() = default;
		std::unordered_map<std::string, unsigned int> m_textures;
	};
}