#include "TextureArray.h"
#include "ImageData.h"
#include "TexturePrivate.h"

#include <format>
#include <stdexcept>

#include <glad/glad.h>
#include <tools/stb_image.h>

using namespace gl::texture;


void TextureArray::create(ArraySettings setting) {
    glGenTextures(1, &m_id);
    glActiveTexture(GL_TEXTURE0 + m_unit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);

    ConfigureTexture(GL_TEXTURE_2D_ARRAY, setting);

    m_maxLayers = setting.layers;
    m_format = setting.format;
    m_width = setting.width;
    m_height = setting.height;

    Data3D(
        GL_TEXTURE_2D_ARRAY,
        setting.width,   // width of each 2D layer
        setting.height,  // height of each 2D layer
        setting.layers,  // number of layers
        setting.format,  // format of the input data
        nullptr          // data pointer (null = reserve space only)
    );
}
int TextureArray::load(const gl::ImageData& imageData, int layer) {
    if (imageData.width != m_width || imageData.height != m_height)
        throw std::runtime_error(
            std::format("Image data size does not match texture size: ({}, {})", m_width, m_height)
        );

    if (imageData.format != m_format)  // todo compatible formats?
        throw std::runtime_error(
            "Image format is different from the one provided when creating this instance"
        );


    if (m_layer == -1 && layer == -1) {
        throw std::runtime_error(
            "Layer must be specified after loading with manual layer management."
        );
    }

    int targetLayer = (layer == -1) ? m_layer++ : layer;
    if (targetLayer >= m_maxLayers)
        throw std::runtime_error("Maximum layers exceeded");
    // glTexSubImage3D(
    //     GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_layer, m_width, m_height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data
    // );
    SubData3D(
        GL_TEXTURE_2D_ARRAY, targetLayer, m_width, m_height, 1, imageData.format, imageData.data
    );

    printf("Layer %d: %s\n", m_layer, imageData.path.c_str());
    return targetLayer;
}

int TextureArray::loadRaw(Data data, int layer) {
    int targetLayer = (layer == -1) ? m_layer++ : layer;
    if (targetLayer >= m_maxLayers)
        throw std::runtime_error("Maximum layers exceeded");

    SubData3D(GL_TEXTURE_2D_ARRAY, targetLayer, m_width, m_height, 1, m_format, data);
    return targetLayer;
}