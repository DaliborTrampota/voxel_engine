#pragma once

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

namespace data {

	class TextureManager
	{
	public:

		void add(std::string &&name, unsigned int id) {
			m_textures.emplace(name, id);
		}

		unsigned int get(std::string name) {
			return m_textures.at(name);
		}

	private:
		std::unordered_map<std::string, unsigned int> m_textures;
	};
}