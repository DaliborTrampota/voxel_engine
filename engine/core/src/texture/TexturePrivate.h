#include "ImageData.h"
#include "TextureBase.h"

#include <glad/glad.h>

namespace gl::texture {
    constexpr GLenum toGLWrap(Settings::Option opt) {
        switch (opt) {
            case Settings::MirroredRepeat: return GL_MIRRORED_REPEAT;
            case Settings::ClampToEdge: return GL_CLAMP_TO_EDGE;
            case Settings::ClampToBorder: return GL_CLAMP_TO_BORDER;
            default: return GL_MIRRORED_REPEAT;
        }
    }

    constexpr GLenum toGLFilter(Settings::Option opt) {
        switch (opt) {
            case Settings::Linear: return GL_LINEAR;
            case Settings::Nearest: return GL_NEAREST;
            default: return GL_LINEAR;
        }
    }

    constexpr GLenum toGLType(Type type) {
        switch (type) {
            case Type::Texture2D: return GL_TEXTURE_2D;
            case Type::Texture3D: return GL_TEXTURE_3D;
            case Type::TextureArray: return GL_TEXTURE_2D_ARRAY;
            case Type::Texture1D: return GL_TEXTURE_1D;
            case Type::TextureArray1D: return GL_TEXTURE_1D_ARRAY;
            case Type::CubeMap: return GL_TEXTURE_CUBE_MAP;
            case Type::CubeMapArray: return GL_TEXTURE_CUBE_MAP_ARRAY;
            case Type::Multisample: return GL_TEXTURE_2D_MULTISAMPLE;
            case Type::MultisampleArray: return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
            case Type::Buffer: return GL_TEXTURE_BUFFER;
        }
        return GL_NONE;
    }

    constexpr GLenum toGLFormat(gl::ImageFormat format) {
        switch (format) {
            case gl::ImageFormat::RGB: return GL_RGB;
            case gl::ImageFormat::RGBA: return GL_RGBA;
            case gl::ImageFormat::GRAY: return GL_RED;
            case gl::ImageFormat::GRAY_ALPHA: return GL_RG;
        }
        return GL_NONE;  // Should not happen
    }

    constexpr GLenum toGLInternalFormat(gl::ImageFormat format) {
        switch (format) {
            case gl::ImageFormat::RGB: return GL_RGB8;
            case gl::ImageFormat::RGBA: return GL_RGBA8;
            case gl::ImageFormat::GRAY: return GL_R8;
            case gl::ImageFormat::GRAY_ALPHA: return GL_RG8;
        }
        return GL_NONE;  // Should not happen
    }

    constexpr GLenum toGLCubeFace(CubeFace face) {
        switch (face) {
            case CubeFace::Right: return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
            case CubeFace::Left: return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
            case CubeFace::Top: return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
            case CubeFace::Bottom: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
            case CubeFace::Back: return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
            case CubeFace::Front: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
        }
        return GL_NONE;  // Should not happen
    }


    void ConfigureTexture(GLenum type, const Settings& settings);
    void Data1D(GLenum type, int width, gl::ImageFormat format, unsigned char* data);
    void Data2D(GLenum type, int width, int height, gl::ImageFormat format, unsigned char* data);
    void Data3D(
        GLenum type, int width, int height, int depth, gl::ImageFormat format, unsigned char* data
    );
    void SubData3D(
        GLenum type,
        int layer,
        int width,
        int height,
        int depth,
        gl::ImageFormat format,
        unsigned char* data
    );
}  // namespace gl::texture
