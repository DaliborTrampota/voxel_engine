# World & Terrain

## Overview

The world system consists of three main abstractions:

- **`World`** — manages a collection of chunks, exposes block get/set, drives terrain generation via a thread pool.
- **`Chunk`** — a fixed-size region of blocks with its own mesh; generated and rendered independently.
- **`ITerrainGenerator`** — interface you implement to describe what block goes at each position.

---

## Chunk Layout

Each chunk is a 3-D grid of blocks. The default dimensions are **16×16×16**, controlled by the static field:

```cpp
engine::Chunk::Dims // glm::ivec3{16, 16, 16}
```

Chunks are identified by their **`ChunkID`** — a thin wrapper over `glm::ivec3` that represents chunk-grid coordinates (not world-space positions). The world-space position of a chunk is:

```
chunkID * chunkDims
```

### Coordinate Utilities

`utility/CoordUtils.h` provides helpers for converting between world space and chunk space:

```cpp
#include <utility/CoordUtils.h>

// Decompose a world position into (chunkID, local position within chunk).
// The pos is modified in place to become chunk-local.
glm::vec3 worldPos = playerPos;
engine::ChunkID chunkID = engine::extractChunkCoords(worldPos);
// worldPos is now [0, chunkDims)

// Get only the chunkID without modifying pos
engine::ChunkID id = engine::getChunkID(playerPos);

// Convert to chunk-local coordinates without extracting
glm::ivec3 local = engine::toChunkCoords(chunkID, playerPos);

// Ray/voxel traversal — returns every block position the ray passes through
std::vector<glm::ivec3> hits = engine::traceLine(start, end);
```

---

## ITerrainGenerator

Implement this interface to define your terrain:

```cpp
#include <level/ITerrainGenerator.h>

class MyGenerator : public engine::ITerrainGenerator {
public:
    // Return the block ID at an absolute world position.
    engine::BlockID voxelAt(const glm::ivec3& pos) override {
        int h = height(pos.x, pos.z);
        if (pos.y < h - 3) return STONE_ID;
        if (pos.y < h)     return DIRT_ID;
        if (pos.y == h)    return GRASS_ID;
        return engine::Block::AirID;
    }

    // Return the surface height for a given xz column.
    // Cache this per column inside populate() to avoid redundant noise calls.
    int height(int x, int z) const override {
        return static_cast<int>(noise.GetNoise(x, z) * 32 + 64);
    }

    // Called once per chunk after voxelAt has populated it.
    // Use this for structures, ores, decoration — anything that needs chunk context.
    void populate(engine::Chunk& chunk) override {
        // chunk.data() gives access to IChunkData
    }

protected:
    FastNoise::SmartNode<> noise = FastNoise::New<FastNoise::Perlin>();
};
```

`m_world` is set automatically before any calls — use it to query other chunks if you need cross-chunk lookups.

---

## World

### Construction

```cpp
#include <level/World.h>

// Default chunk dims (Chunk::Dims), 8 generation threads
auto world = std::make_shared<engine::World>(
    std::make_unique<MyGenerator>(),
    8 // generation thread count
);

// Custom chunk dims
auto world = std::make_shared<engine::World>(
    std::make_unique<MyGenerator>(),
    glm::ivec3(32, 32, 32),
    8
);
```

Register it with the engine so it gets update calls and can be rendered:

```cpp
engine.subscribeUpdate(world);
engine.subscribeTick(world);   // if your World subclass implements Tickable
```

### Loading and Unloading Chunks

```cpp
// Load all chunks in a box from chunkID (0,0,0) to (15,0,15).
// Returns a std::future<void> that becomes ready when all chunks are generated.
auto done = world->loadChunks(glm::ivec3(0,0,0), glm::ivec3(15,0,15));
done.wait(); // optional — block until ready

// Load a range and unload everything outside it
world->loadChunks(from, to, /*unloadRest=*/true);

// Unload a specific range
world->unloadChunks(from, to);

// Unload everything except a given list
world->unloadAllChunks(/* except = */ { keepChunk1, keepChunk2 });
```

Chunk generation runs on the `World`'s internal thread pool (`m_genPool`). Chunks are generated in the background; the world renders whatever is ready each frame.

### Block Access

Two addressing styles are available — **chunk-local** (chunk ID + position within chunk) and **world-space** (absolute `glm::ivec3`/`glm::vec3`):

```cpp
// World-space (convenience — resolves chunk automatically)
world->setBlock(glm::ivec3(10, 64, 5), GRASS_ID);
world->setBlock(glm::ivec3(10, 64, 5), GRASS_ID, state);

engine::BlockID id = world->getBlockID(
    glm::vec3(10, 64, 5),
    /*fallbackToGenerator=*/true  // query generator if chunk not yet loaded
);

// Chunk-local
world->setBlock(chunkID, localPos, DIRT_ID);
engine::BlockID id = world->getBlockID(chunkID, localPos, false);
```

`setBlock` automatically marks the chunk's mesh as dirty and triggers a mesh rebuild. It also calls `afterBlockSet` (virtual hook — override in your `World` subclass).

### Chunk Access

```cpp
std::shared_ptr<engine::Chunk> ch = world->getChunk(chunkID);

// If you have a custom chunk subclass:
std::shared_ptr<MyChunk> ch = world->getChunkAs<MyChunk>(chunkID);
```

### Skybox

Each `World` has an embedded `Skybox`:

```cpp
engine::Skybox::Settings s = {
    "skybox/right.jpg", "skybox/left.jpg",
    "skybox/top.jpg",   "skybox/bottom.jpg",
    "skybox/front.jpg", "skybox/back.jpg"
};
world->getSkybox().load(s);
```

---

## Subclassing World

Override `createChunk` to inject a custom `Chunk` subclass:

```cpp
class MyWorld : public engine::World {
public:
    using World::World;

protected:
    std::shared_ptr<engine::Chunk> createChunk(const engine::ChunkID& id) override {
        return std::make_shared<MyChunk>(this, id, /* IChunkData */);
    }

    // Called after any block is set via World::setBlock
    void afterBlockSet(
        const glm::ivec3& pos, engine::BlockID id, engine::BlockState* state
    ) override {
        // trigger game-specific logic here
    }
};
```

---

## IChunkData

`IChunkData` is the storage backend for a chunk. The default implementation (`DenseGrid`) stores a full 3-D array. You can provide your own by implementing:

```cpp
class IChunkData : public ISerializable {
public:
    glm::ivec3 dims;
    bool populated = false;  // set to true in ChunkBeforeLoadEvent to skip generator

    virtual BlockID getBlock(const glm::ivec3& pos) const = 0;
    virtual void setBlock(const glm::ivec3& pos, BlockID block) = 0;
    virtual void setBlock(const glm::ivec3& pos, BlockID block, BlockState state) = 0;

    virtual BlockState* getState(const glm::ivec3& pos) = 0;
    virtual void setState(const glm::ivec3& pos, BlockState&& state) = 0;
    virtual void clearState(const glm::ivec3& pos) = 0;

    // MultiBlock support
    virtual MultiBlock* getMultiBlock(const glm::ivec3& pos) = 0;
    virtual void setMultiBlock(const glm::ivec3& pos, MultiBlock&& block) = 0;
};
```

Access it from inside `ITerrainGenerator::populate` or a custom `Chunk`:

```cpp
chunk.data()->setBlock(localPos, STONE_ID);
bool empty = chunk.data()->isEmpty(localPos);
```

---

**See also:** [Events & Input](events-input.md) for chunk load/unload events fired by `World`.
