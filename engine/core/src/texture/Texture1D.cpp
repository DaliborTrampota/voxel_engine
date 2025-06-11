#include "Texture1D.h"
#include "ImageData.h"
#include "TexturePrivate.h"

#include <format>
#include <stdexcept>

using namespace gl::texture;


void Texture1D::create(Settings settings) {
    if (m_id != 0)
        throw std::runtime_error("Texture1D already created");

    glGenTextures(1, &m_id);
    glActiveTexture(GL_TEXTURE0 + m_unit);
    glBindTexture(GL_TEXTURE_1D, m_id);

    ConfigureTexture(GL_TEXTURE_1D, settings);
}

void Texture1D::load(const gl::ImageData& imageData) {
    m_width = imageData.width;
    m_channels = imageData.channels;

    Data1D(GL_TEXTURE_1D, imageData.width, imageData.format, imageData.data);
    // glTexImage1D(
    //     GL_TEXTURE_1D,
    //     0,                 // mipmap level
    //     toGLInternalFormat(imageData.format),          // internal format
    //     imageData.width,   // width of the texture
    //     0,                 // border (must be 0)
    //     toGLFormat(imageData.format),  // format of the input data
    //     GL_UNSIGNED_BYTE,  // type of the input data
    //     imageData.data     // data pointer (null = reserve space only)
    // );
}

void Texture1D::loadRaw(int w, int ch, gl::ImageFormat format, Data data) {
    m_width = w;
    m_channels = ch;

    Data1D(
        GL_TEXTURE_1D,
        w,       // width of the texture
        format,  // format of the input data
        data     // data pointer (null = reserve space only)
    );
}