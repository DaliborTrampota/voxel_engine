#include "TextureLoader.h"

#include <filesystem>
#include <vector>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <tools/stb_image.h>
#include <glad/glad.h>

#include "data/TextureManager.h"

namespace fs = std::filesystem;

using namespace engine;

TextureLoader::TextureLoader(int slot, gl::TextureSettings settings)
    : m_texArray(slot, settings)
{
}

void TextureLoader::load(const char* path) {

    m_texArray.bind();

    
    std::vector<std::string> paths;
    int width = 0;
    int height = 0;
    bool query = true;

    try {
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
    }
    catch (std::filesystem::filesystem_error& e) {
        printf("Error loading texutes: %s\n", e.what());
        exit(0);
    }
    if (width == 0 || height == 0) {
        printf("No valid images found in %s\n", path);
        return;
    }
    
    m_texArray.create(width, height, paths.size());
    TextureManager& texMgr = TextureManager::Get();
    for (const auto& p : paths)
    {
        int layer = m_texArray.load(p.c_str());
        texMgr.add(getTextureName(p), layer);
    }
}

void TextureLoader::bind() const
{
    m_texArray.bind();
}

std::string TextureLoader::getTextureName(const std::string& path)
{
    int idx = path.find_last_of("/") + 1;
    int count = path.find_last_of(".") - idx;
    return path.substr(idx, count);
}