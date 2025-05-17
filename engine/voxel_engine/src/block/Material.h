#pragma once

#include <array>

#include "Face.h"
#include "../Globals.h"

namespace engine {

    struct Material
    {
        std::array<TexID, static_cast<size_t>(FaceTag::Count)> m_textures{};
        // std::unordered_map<std::string, TexID> textures;

        TexID forTag(FaceTag tag) const
        {
            return m_textures[static_cast<size_t>(tag)];
        }

        Material& add(FaceTag tag, TexID texture)
        {
            if (tag == FaceTag::All) {
                for(int i = 0; i < static_cast<size_t>(FaceTag::Count); i++)
                    m_textures[i] = texture;
                return *this;
            }

            if (tag == FaceTag::Side) {
                for (int i = static_cast<size_t>(FaceTag::Right); i <= static_cast<size_t>(FaceTag::Back); i++)
                    m_textures[i] = texture;
            }

            m_textures[static_cast<size_t>(tag)] = texture;

            return *this;
        }
    };
}