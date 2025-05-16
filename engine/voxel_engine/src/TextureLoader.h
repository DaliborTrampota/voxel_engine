#pragma once

#include <string>

namespace engine 
{
	class TextureLoader
	{
	public:
		TextureLoader();

		void load(const char* path);
		void bind(int slot = 0) const;

	private:

		std::string getTextureName(const std::string *path) const;

		unsigned int m_texture;
	};

}

