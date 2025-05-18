#pragma once

#include <string>

#include <core/gl/TextureArray.h>

namespace engine 
{

    inline constexpr gl::TextureSettings pixelFormat = {
        gl::TextureSettings::MirroredRepeat,
        gl::TextureSettings::MirroredRepeat,
        gl::TextureSettings::Nearest,
        gl::TextureSettings::Nearest
    };

	class TextureLoader
	{
	public:
		TextureLoader(int slot, gl::TextureSettings settings = pixelFormat);

		void load(const char* dirPath);
		void bind() const;

	private:
        gl::TextureArray m_texArray;

		static std::string getTextureName(const std::string& path);

	};

}

