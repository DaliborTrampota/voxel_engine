#include "TextureLoader.h"

#include <glad/glad.h>
#include <filesystem>
#include <vector>
#include <string>

#include <stb_image.h>

#include "data/TextureManager.h"

namespace fs = std::filesystem;

namespace asset {

	TextureLoader::TextureLoader()
	{
		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}


	void TextureLoader::load(const char* path, data::TextureManager *manager) {

		glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);

		
		std::vector<std::string> paths;
		int width = 0;
		int height = 0;
		bool query = true;


		for (const fs::directory_entry entry : fs::directory_iterator(path))
		{
			if (!entry.is_regular_file()) {
				printf("%s is not a regular file\n", entry.path().string().c_str());
				continue;
			}

			int n, w, h;
			std::string p = entry.path().string();
			int ok = stbi_info(p.c_str(), &w, &h, nullptr);
			if (ok == 1) {
				paths.push_back(p);
				width = w;
				height = h;
			}
			else {
				printf("Could not load %s\n", entry.path().string().c_str());
			}
		}
		if (width == 0 || height == 0) {
			printf("No valid images found in %s\n", path);
			return;
		}
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, width, height, paths.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		int layer = 0;
		for (const auto& p : paths)
		{
			int cWidth, cHeight, cChannels;
			stbi_uc* data = stbi_load(p.c_str(), &cWidth, &cHeight, &cChannels, 0);
			if (!data || cWidth != width || cHeight != height) {
				printf("%s is not the size of (%d, %d)", p.c_str(), width, height);
				continue;
			}

			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
			//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			printf("Layer %d: %s\n", layer, p.c_str());
			manager->add(getTextureName(&p), layer);

			stbi_image_free(data);
			++layer;
		}
	}

	void TextureLoader::bind(int slot) const
	{
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);
	}

	std::string TextureLoader::getTextureName(const std::string *path) const
	{
		int idx = path->find_last_of("/") + 1;
		int count = path->find_last_of(".") - idx;
		return path->substr(idx, count).c_str();
	}


} // namespace tex