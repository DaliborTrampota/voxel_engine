#include "TextureLoader.h"

#include <filesystem>
#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <LWGL/texture/ImageData.h>
#include <glad/glad.h>
#include <tools/stb_image.h>

#include "data/TextureManager.h"

namespace fs = std::filesystem;

using namespace engine;

TextureLoader::TextureLoader(int slot) : m_texArray(slot) {}

void TextureLoader::load(const char* path, gl::ArraySettings settings) {
    int width = 0;
    int height = 0;
    std::vector<gl::ImageData> images;

    if (!fs::exists(path)) {
        printf("Path %s does not exist\n", path);
        return;
    }

    try {
        for (const fs::directory_entry& entry : fs::directory_iterator(path)) {
            if (!entry.is_regular_file()) {
                printf("%s is not a regular file\n", entry.path().string().c_str());
                continue;
            }

            gl::ImageData data(entry.path().string().c_str());
            width = data.width;
            height = data.height;
            images.emplace_back(std::move(data));
        }
    } catch (std::filesystem::filesystem_error& e) {
        printf("Error loading texutes: %s\n", e.what());
        exit(0);
    }

    if (width == 0 || height == 0) {
        printf("No valid images found in %s\n", path);
        return;
    }


    settings.layers = images.size();
    settings.width = width;
    settings.height = height;
    settings.format = gl::ImageFormat::RGBA;


    m_texArray.create(settings);
    m_texArray.bind();
    TextureManager& texMgr = TextureManager::Get();

    for (const auto& imgData : images) {
        int layer = m_texArray.load(imgData);
        texMgr.add(getTextureName(imgData.path), layer);
    }
}

void TextureLoader::bind() const {
    m_texArray.bind();
}

std::string TextureLoader::getTextureName(const std::string& path) {
    size_t idx = path.find_last_of("/") + 1;
    size_t count = path.find_last_of(".") - idx;
    return path.substr(idx, count);
}