#pragma once

#include <string>

namespace data {
	class TextureManager;
}

namespace asset 
{

	class TextureLoader
	{
	public:
		TextureLoader();

		void load(const char* path, data::TextureManager *manager);
		void bind(int slot = 0) const;

	private:

		std::string getTextureName(const std::string *path) const;

		unsigned int m_texture;
	};

}

