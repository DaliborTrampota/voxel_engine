#pragma once


namespace gl
{

    struct TextureSettings
    {
        enum Option {
            MirroredRepeat,
            ClampToEdge,
            ClampToBorder,
            Linear,
            Nearest,
        };

        Option wrapS = MirroredRepeat;
        Option wrapT = MirroredRepeat;
        Option minFilter = Linear;
        Option magFilter = Linear;
    };

    enum class TextureType
    {
        Texture2D,
        Texture3D,
        TextureArray,
    };

    class TextureArray
    {
        public:
            TextureArray(unsigned int unit, TextureSettings setting = TextureSettings());
            ~TextureArray();

            void create(unsigned int width, unsigned int height, unsigned int layers);
            int load(const char* path);

            unsigned int id() const { return m_ID; }
            void bind() const;
            void unbind() const;

        private:
            unsigned int m_ID;

            unsigned int m_unit;
            unsigned int m_layer = 0;
            unsigned int m_maxLayers = 0;
            unsigned int m_width, m_height;
    };
}