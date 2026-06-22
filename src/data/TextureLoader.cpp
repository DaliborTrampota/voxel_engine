#include "TextureLoader.h"

#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <LWGL/texture/ImageData.h>
#include <glad/glad.h>
#include <tools/stb_image.h>

#include "data/TextureManager.h"

using namespace engine;

void TextureLoader::loadArray2D(
    gl::TextureArray* target, const fs::path& path, gl::TextureParams params
) {
    int width = 0;
    int height = 0;
    std::vector<gl::ImageData> images;

    if (!fs::exists(path)) {
        printf("Path %s does not exist\n", path.string().c_str());
        return;
    }

    try {
        for (const fs::directory_entry& entry : fs::directory_iterator(path)) {
            if (!entry.is_regular_file()) {
                printf("%s is not a regular file\n", entry.path().string().c_str());
                continue;
            }

            gl::ImageData data(entry.path().string().c_str());
            width = std::max(width, data.width);
            height = std::max(height, data.height);
            images.emplace_back(std::move(data));
        }
    } catch (std::filesystem::filesystem_error& e) {
        printf("Error loading texutes: %s\n", e.what());
        exit(0);
    }

    if (width == 0 || height == 0) {
        printf("No valid images found in %s\n", path.string().c_str());
        return;
    }


    target->create(params);
    target->allocate(
        {.width = width,
         .height = height,
         .depth = static_cast<int>(images.size()),
         .format = gl::ImageFormat::RGBA}
    );
    TextureManager& texMgr = TextureManager::Get();
    for (const auto& imgData : images) {
        int layer = target->upload(imgData);
        texMgr.add(getTextureName(imgData.path), layer);
    }
}

std::string TextureLoader::getTextureName(const fs::path& path) {
    return path.filename().replace_extension("").string();
}