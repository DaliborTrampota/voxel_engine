#pragma once

namespace tex 
{


	class TextureLoader
	{
	public:
		TextureLoader();

		void load(const char* path);
		void bind(int slot = 0) const;

	private:
		unsigned int m_texture;
	};

}

