#include "CubeMap.h"
#include "ImageData.h"
#include "TexturePrivate.h"

#include <glad/glad.h>

using namespace gl::texture;

void CubeMap::create(Settings setting) {
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

    ConfigureTexture(GL_TEXTURE_CUBE_MAP, setting);
}

void CubeMap::loadFace(CubeFace face, const gl::ImageData& data) {
    if (data.width != data.height)
        throw std::runtime_error("Cube map face must be square");

    m_width = data.width;
    m_channels = data.channels;

    Data2D(toGLCubeFace(face), data.width, data.height, data.format, data.data);
}

void CubeMap::loadFaceRaw(CubeFace face, int w, int ch, gl::ImageFormat format, Data data) {
    if (w <= 0 || ch <= 0)
        throw std::runtime_error("Width and height must be greater than zero");

    m_width = w;
    m_channels = ch;

    Data2D(
        toGLCubeFace(face),
        w,
        w,  // Cube map faces are square
        format,
        data
    );
}
