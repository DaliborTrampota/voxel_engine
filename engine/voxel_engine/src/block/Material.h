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
            size_t tagIdx = static_cast<size_t>(tag);
            TexID id = m_textures[tagIdx];
            
            if (!id && tagIdx >= static_cast<size_t>(FaceTag::Right) && tagIdx <= static_cast<size_t>(FaceTag::Back))
                id = m_textures[static_cast<size_t>(FaceTag::Side)];

            if (!id)
                id = m_textures[static_cast<size_t>(FaceTag::All)];

            return id;
        }

        Material& add(FaceTag tag, TexID texture)
        {
            m_textures[static_cast<size_t>(tag)] = texture;

            return *this;
        }
    };
}