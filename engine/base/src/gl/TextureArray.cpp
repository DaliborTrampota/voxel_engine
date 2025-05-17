#include "TextureArray.h"

#include <stdexcept>
#include <format>

#include <glad/glad.h>
#include <tools/stb_image.h>

using namespace gl;


namespace {
    constexpr GLenum toGLWrap(TextureSettings::Option opt) {
        switch (opt) {
            case TextureSettings::MirroredRepeat: return GL_MIRRORED_REPEAT;
            case TextureSettings::ClampToEdge: return GL_CLAMP_TO_EDGE;
            case TextureSettings::ClampToBorder: return GL_CLAMP_TO_BORDER;
            default: return GL_MIRRORED_REPEAT;
        }
    }

    constexpr GLenum toGLFilter(TextureSettings::Option opt) {
        switch (opt) {
            case TextureSettings::Linear: return GL_LINEAR;
            case TextureSettings::Nearest: return GL_NEAREST;
            default: return GL_LINEAR;
        }
    }

    constexpr GLenum toGLType(TextureType type) {
        switch (type) {
            case TextureType::Texture2D: return GL_TEXTURE_2D;
            case TextureType::Texture3D: return GL_TEXTURE_3D;
            case TextureType::TextureArray: return GL_TEXTURE_2D_ARRAY;
        }
        return GL_TEXTURE_2D;
    }
}

TextureArray::TextureArray(unsigned int unit, TextureSettings setting)
    : m_unit(unit)
{
    // if (unit >= 32)
    //     throw std::runtime_error("Texture unit out of range");

    glGenTextures(1, &m_ID);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_ID);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, toGLWrap(setting.wrapS));
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, toGLWrap(setting.wrapT));

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, toGLFilter(setting.minFilter));
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, toGLFilter(setting.magFilter));

}

TextureArray::~TextureArray()
{
    glDeleteTextures(1, &m_ID);
}

void TextureArray::create(unsigned int width, unsigned int height, unsigned int layers)
{
    m_width = width;
    m_height = height;
    m_maxLayers = layers;

    glTexImage3D(GL_TEXTURE_2D_ARRAY,
        0,                          // mipmap level
        GL_RGBA8,                  // internal format
        m_width,                     // width of each 2D layer
        m_height,                    // height of each 2D layer
        layers,                // number of layers
        0,                         // border (must be 0)
        GL_RGBA,                   // format of the input data
        GL_UNSIGNED_BYTE,          // type of the input data
        nullptr                    // data pointer (null = reserve space only)
    );
}

int TextureArray::load(const char* path)
{
    int cWidth, cHeight, cChannels;
    stbi_uc* data = stbi_load(path, &cWidth, &cHeight, &cChannels, 0);
    if(!data)
        throw std::runtime_error(std::format("Failed to load texture: {}", stbi_failure_reason()));

    if (cWidth != m_width || cHeight != m_height)
        throw std::runtime_error(std::format("{} is not the size of ({}, {})", path, m_width, m_height));

    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_layer, m_width, m_height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
    printf("Layer %d: %s\n", m_layer, path);

    stbi_image_free(data);
    return m_layer++;
}


void TextureArray::bind() const
{
    glActiveTexture(GL_TEXTURE0 + m_unit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_ID);
}

void TextureArray::unbind() const
{

}
