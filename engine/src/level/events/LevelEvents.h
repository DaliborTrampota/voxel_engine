#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "utility/Event.h"


namespace engine {

    class Chunk;

    struct ChunkUnloadEvent : public Event {
        ChunkUnloadEvent(std::shared_ptr<const Chunk> _chunk) : chunk(_chunk) {}

        /// @brief The chunk that is about to be unloaded.
        std::shared_ptr<const Chunk> chunk;
    };

    struct ChunkBeforeLoadEvent : public Event {
        ChunkBeforeLoadEvent(std::shared_ptr<Chunk> _chunk) : chunk(_chunk) {}

        /// @brief The chunk that is about to be loaded.
        /// @note Set the chunk->data().populated to true to skip terrain population from generator (eg. for chunk serialization)
        std::shared_ptr<Chunk> chunk;
    };

    struct ChunkRangeUpdatedEvent : public Event {
        ChunkRangeUpdatedEvent(const glm::ivec3& _from, const glm::ivec3& _to)
            : from(_from),
              to(_to) {}

        /// @brief The chunk coordinate start.
        const glm::ivec3& from;
        /// @brief The chunk coordinate end.
        const glm::ivec3& to;
    };
}  // namespace engine
