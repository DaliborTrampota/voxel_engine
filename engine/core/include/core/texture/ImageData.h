#pragma once

#include <tools/stb_image.h>
#include <format>
#include <stdexcept>
#include <string>

namespace gl {

    enum class ImageFormat {
        Unknown,
        RGB,        // Red, Green, Blue
        RGBA,       // Red, Green, Blue, Alpha
        GRAY,       // Grayscale
        GRAY_ALPHA  // Grayscale with Alpha
    };

    struct ImageData {
        int width = 0;
        int height = 0;
        int channels = 0;
        unsigned char* data = nullptr;
        ImageFormat format = ImageFormat::RGBA;
        std::string path;

        ImageData(const ImageData&) = delete;
        ImageData& operator=(const ImageData&) = delete;

        ~ImageData() { stbi_image_free(data); }

        ImageData(const char* path) {
            data = stbi_load(path, &width, &height, &channels, 0);
            if (!data)
                throw std::runtime_error(
                    std::format("Failed to load image: {}", stbi_failure_reason())
                );

            this->path = path;

            switch (channels) {
                case 1: format = ImageFormat::GRAY; break;
                case 2: format = ImageFormat::GRAY_ALPHA; break;
                case 3: format = ImageFormat::RGB; break;
                case 4: format = ImageFormat::RGBA; break;
                default: throw std::runtime_error("Unsupported number of channels");
            }
        }


        // Implement move constructor and move assignment to transfer ownership safely
        ImageData(ImageData&& other) noexcept
            : width(other.width),
              height(other.height),
              channels(other.channels),
              data(other.data),
              format(other.format),
              path(other.path) {
            other.data = nullptr;  // Take ownership
        }

        ImageData& operator=(ImageData&& other) noexcept {
            if (this != &other) {
                stbi_image_free(data);  // free existing data

                width = other.width;
                height = other.height;
                channels = other.channels;
                data = other.data;
                format = other.format;
                path = other.path;

                other.data = nullptr;  // take ownership
            }
            return *this;
        }
    };
}  // namespace gl