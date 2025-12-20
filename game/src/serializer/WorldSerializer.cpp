#include "WorldSerializer.h"

#include <iostream>

#include <level/Chunk.h>

void WorldSerializer::serialize(const engine::Chunk& chunk) {
    std::cout << "(unload) Serializing chunk " << chunk.id() << std::endl;
}

void WorldSerializer::deserialize(engine::Chunk& chunk) {
    std::cout << "(load) Deserializing chunk " << chunk.id() << std::endl;
}