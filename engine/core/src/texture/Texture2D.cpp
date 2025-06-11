#include "Texture2D.h"
#include "ImageData.h"
#include "TexturePrivate.h"

#include <glad/glad.h>
#include <format>
#include <stdexcept>

using namespace gl::texture;


void Texture2D::create(Settings settings) {
    if (m_id != 0)
        throw std::runtime_error("Texture2D already created");

    glGenTextures(1, &m_id);
    glActiveTexture(GL_TEXTURE0 + m_unit);
    glBindTexture(GL_TEXTURE_2D, m_id);

    ConfigureTexture(GL_TEXTURE_2D, settings);
}

void Texture2D::load(const gl::ImageData& imageData) {
    m_width = imageData.width;
    m_height = imageData.height;
    m_channels = imageData.channels;

    Data2D(GL_TEXTURE_2D, imageData.width, imageData.height, imageData.format, imageData.data);
    // glTexImage2D(
    //     GL_TEXTURE_2D,
    //     0,                 // mipmap level
    //     GL_RGBA8,          // internal format
    //     imageData.width,           // width of each 2D layer
    //     imageData.height,          // height of each 2D layer
    //     0,                 // border (must be 0)
    //     toGLFormat(imageData.format),           // format of the input data
    //     GL_UNSIGNED_BYTE,  // type of the input data
    //     imageData.data            // data pointer (null = reserve space only)
    // );
}

void Texture2D::loadRaw(int w, int h, int ch, gl::ImageFormat format, Data data) {
    m_width = w;
    m_height = h;
    m_channels = ch;

    Data2D(
        GL_TEXTURE_2D,
        w,       // width of the texture
        h,       // height of the texture
        format,  // format of the input data
        data     // data pointer (null = reserve space only)
    );
}