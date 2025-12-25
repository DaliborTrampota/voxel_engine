#include "WorldSerializer.h"

#include <fstream>
#include <iostream>

#include <level/Chunk.h>

#include "../Directory.h"

namespace {
    fs::path s_chunksDirectory;
}

WorldSerializer::WorldSerializer() {
    s_chunksDirectory = getUserDataDirectory() / "chunks";
    fs::create_directories(s_chunksDirectory);
}

//TODO RLE
void WorldSerializer::serialize(std::shared_ptr<const engine::Chunk> chunk) {
    std::ofstream file(s_chunksDirectory / (chunk->id().toString() + ".chunk"), std::ios::binary);
    if (!file.is_open()) {
        std::cout << "(unload) Failed to open file for chunk " << chunk->id() << std::endl;
        return;
    }

    file << engine::Chunk::Dims.x << " " << engine::Chunk::Dims.y << " " << engine::Chunk::Dims.z
         << std::endl;

    engine::ChunkData data = chunk->data();
    for (int x = 0; x < engine::Chunk::Dims.x; x++) {
        for (int y = 0; y < engine::Chunk::Dims.y; y++) {
            for (int z = 0; z < engine::Chunk::Dims.z; z++) {
                file << chunk->data().getBlock(x, y, z) << " ";
            }
        }
    }
    file.close();
}

void WorldSerializer::deserialize(std::shared_ptr<engine::Chunk> chunk) {
    std::ifstream file(s_chunksDirectory / (chunk->id().toString() + ".chunk"), std::ios::binary);
    if (!file.is_open()) {
        //std::cout << "(load) Chunk " << chunk->id() << " not found" << std::endl;
        return;
    }

    int dims[3];
    file >> dims[0] >> dims[1] >> dims[2];
    engine::ChunkData data(glm::ivec3(dims[0], dims[1], dims[2]));
    data.populated = true;

    engine::BlockID block;
    for (int x = 0; x < dims[0]; x++) {
        for (int y = 0; y < dims[1]; y++) {
            for (int z = 0; z < dims[2]; z++) {
                file >> block;
                data.setBlock(x, y, z, block);
            }
        }
    }
    file.close();
    chunk->data() = data;
}