#pragma once

#include <array>

#include "../Globals.h"
#include "Face.h"
#include "Side.h"

namespace engine {

    struct BlockMaterial {
        std::array<TexID, static_cast<size_t>(FaceTag::Count)> m_textures{};
        // std::unordered_map<std::string, TexID> textures;

        TexID forTag(FaceTag tag) const { return m_textures[static_cast<size_t>(tag)]; }

        BlockMaterial& add(FaceTag tag, TexID texture) {
            if (tag == FaceTag::All) {
                for (int i = 0; i < static_cast<size_t>(FaceTag::Count); i++)
                    m_textures[i] = texture;
                return *this;
            }

            if (tag == FaceTag::Side) {
                for (auto face : IterateXZFaces)
                    m_textures[static_cast<size_t>(face)] = texture;
            }

            m_textures[static_cast<size_t>(tag)] = texture;

            return *this;
        }
    };
}  // namespace engine