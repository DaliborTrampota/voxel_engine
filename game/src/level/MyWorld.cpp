#include "MyWorld.h"

#include "../Player.h"
#include "CoordUtils.h"
#include "level/Chunk.h"

#include <render/Engine.h>
#include <algorithm>
#include <glm/gtx/norm.hpp>

using namespace engine;

void MyWorld::render(Engine& engine, const Camera* camera, int pass) {
    glm::vec3 playerPos = m_player->position();
    glm::vec3 curChunk = extractChunkCoords(playerPos);

    std::vector<ChunkID> renderOrder(m_loadedChunks.begin(), m_loadedChunks.end());
    // clang-format off
    std::sort(
        renderOrder.begin(), renderOrder.end(), [&curChunk](const ChunkID& a, const ChunkID& b) {
            return glm::length2(curChunk - glm::vec3(a)) < glm::length2(curChunk - glm::vec3(b));
        }
    );
    // clang-format on

    //for(const ChunkID& pos : renderOrder) {// Opaque front to back
    //    m_chunks.at(pos)->render(engine, 1);
    //}

    //std::reverse(renderOrder.begin(), renderOrder.end());
    for (const ChunkID& pos : renderOrder) {
        m_chunks.at(pos)->render(engine, camera, 0);
    }
}

void MyWorld::setPlayer(std::shared_ptr<Player> player) {
    m_player = player;
}
