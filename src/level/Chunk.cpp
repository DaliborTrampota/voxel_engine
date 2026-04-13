#include "Chunk.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <memory>
#include <ranges>


#include <LWGL/buffer/Attributes.h>

#include "World.h"
#include "block/Block.h"
#include "block/MultiBlock.h"
#include "block/Side.h"
#include "block/VariantBlock.h"
#include "block/Vertex.h"
#include "data/RegistryManager.h"
#include "render/Engine.h"
#include "render/RenderContext.h"
#include "utility/Rotation.h"


using namespace engine;

namespace {
    RegistryManager::BlockRegistryT& blocks = RegistryManager::Blocks();
}

Chunk::Chunk(World* world, ChunkID coords, std::unique_ptr<IChunkData> data)
    : m_world(world),
      m_coords(coords),
      m_data(std::move(data)) {}

Chunk::~Chunk() {}

void Chunk::populateTerrainData() {
    if (m_generated || m_data->populated)
        return;

    m_world->m_generator->populate(*this);
    m_data->populated = true;
}

bool Chunk::generateMesh() {
    bool expected = false;
    if (!m_generatingMesh.compare_exchange_strong(expected, true))
        return false;

    glm::ivec3 chunkBlockCoords = m_coords * m_data->dims;

    for (auto& layer : m_renderLayers) {
        layer.clear();
        layer.back.reserve(m_data->dims.x * m_data->dims.y * 8 * 6);
    }

    for (int x = 0; x < m_data->dims.x; x++) {
        for (int y = 0; y < m_data->dims.y; y++) {
            for (int z = 0; z < m_data->dims.z; z++) {
                if (m_data->getBlock({x, y, z}) == Block::AirID)
                    continue;

                glm::ivec3 pos(x, y, z);

                BlockID blockID = m_data->getBlock(pos);
                const Block* block = RegistryManager::Blocks().get(blockID);

                if (!block) {
                    // Block not found in registry, skip, TODO render pink/black block missing texture?
                    continue;
                }

                const BlockState* state = m_data->getState(pos);

                if (auto variant = dynamic_cast<const VariantBlock*>(block)) {
                    generateMeshForBlock(variant, pos, state, chunkBlockCoords);
                } else if (block->isMultiblock()) {
                    MultiBlock* multi = m_data->getMultiBlock(pos);
                    generateMeshForBlock(multi, pos, state, chunkBlockCoords);
                } else {
                    generateMeshForBlock(block, pos, state, chunkBlockCoords);
                }
            }
        }
    }

    for (auto& layer : m_renderLayers) {
        layer.moveToFront();
    }

    m_generated = true;
    m_generatingMesh = false;
    afterGenerated();
    return true;
}

const Block* Chunk::getBlock(glm::ivec3 pos, BlockState** state) {
    BlockID blockID = !m_data->populated
                          ? m_world->m_generator->voxelAt(pos + m_coords * m_data->dims)
                      : state ? m_data->getBlockAndState(pos, *state)
                              : m_data->getBlock(pos);
    return RegistryManager::Blocks().get(blockID);
}

void Chunk::setBlock(const glm::ivec3& pos, BlockID block) {
    BlockState* oldState = m_data->getState(pos);
    BlockID oldBlock = m_data->getBlock(pos);
    if (oldBlock != Block::AirID)
        blocks.get(oldBlock)->onDestroyed(
            BlockSetContext{.chunk = this, .position = pos, .state = oldState}
        );

    m_data->setBlock(pos, block);
    if (block == Block::AirID) {
        m_data->clearState(pos);
    }

    blocks.get(block)->onPlaced(BlockSetContext{.chunk = this, .position = pos, .state = nullptr});
    m_world->afterBlockSet(pos, block, nullptr);
    m_dirty = true;
}

void Chunk::setBlock(const glm::ivec3& pos, BlockID block, BlockState state) {
    BlockState* oldState = m_data->getState(pos);
    BlockID oldBlock = m_data->getBlock(pos);
    if (oldBlock != Block::AirID)
        blocks.get(oldBlock)->onDestroyed(
            BlockSetContext{.chunk = this, .position = pos, .state = oldState}
        );

    m_data->setBlock(pos, block, state);
    if (block == Block::AirID) {
        m_data->clearState(pos);
    }

    BlockState* setState = m_data->getState(pos);
    blocks.get(block)->onPlaced(BlockSetContext{.chunk = this, .position = pos, .state = setState});
    m_world->afterBlockSet(pos, block, setState);
    m_dirty = true;
}

void Chunk::setBlock(const glm::ivec3& pos, MultiBlock&& multiBlock) {
    m_data->setMultiBlock(pos, std::move(multiBlock));

    // TODO onPlaced?
    m_world->afterBlockSet(pos, Block::MultiblockID, nullptr);
    m_dirty = true;
}


void Chunk::render(Engine& engine, const Camera* camera, int pass) {
    if (!m_generated) {
        return;
    }

    RenderContext ctx;
    ctx.setModelMatrix(m_coords * m_data->dims);
    if (pass == 0) {
        ctx.attributes = &m_renderLayers[Layers::Opaque].read();
        ctx.material = &m_world->m_material;
        ctx.passMask = RenderPass::Scene | RenderPass::DirectionalShadow;
        ctx.camera = camera;
        engine.submitRender(std::move(ctx));

        RenderContext ctxTransparent;
        ctxTransparent.setModelMatrix(m_coords * m_data->dims);
        ctxTransparent.attributes = &m_renderLayers[Layers::Transparent].read();
        ctxTransparent.material = &m_world->m_material;
        ctxTransparent.passMask = RenderPass::SceneTransparent | RenderPass::DirectionalShadow;
        ctxTransparent.camera = camera;
        engine.submitRender(std::move(ctxTransparent));
    }

    else if (pass == 1) {  // Opaque front to back
        if (!m_renderLayers[Layers::Opaque].read().length())
            return;
        ctx.attributes = &m_renderLayers[Layers::Opaque].read();

        ctx.material = &m_world->m_material;
        ctx.passMask = RenderPass::Scene | RenderPass::DirectionalShadow;
        ctx.camera = camera;
        engine.submitRender(std::move(ctx));
    }

    else if (pass == 2) {  // Transparent back to front
        if (!m_renderLayers[Layers::Transparent].read().length())
            return;
        ctx.attributes = &m_renderLayers[Layers::Transparent].read();

        ctx.material = &m_world->m_material;
        ctx.passMask = RenderPass::SceneTransparent | RenderPass::DirectionalShadow;
        ctx.camera = camera;
        engine.submitRender(std::move(ctx));
    }
}

Chunk::GeometryState Chunk::calculateGeometryState(
    const Block* block, const BlockState* state
) const {
    float angle = 0.0f;
    glm::vec3 axis = glm::vec3(0, 1, 0);
    calculateRotationFromState(state, block, angle, axis);
    return GeometryState{
        .axis = axis,
        .angle = angle,
    };
}


bool ChunkID::operator==(const ChunkID& other) const {
    return x == other.x && y == other.y && z == other.z;
}

std::ostream& operator<<(std::ostream& os, const ChunkID& chID) {
    return os << "ChunkID(" << chID.x << ", " << chID.y << ", " << chID.z << ")";
}

std::string ChunkID::toString() const {
    return std::to_string(x) + "_" + std::to_string(y) + "_" + std::to_string(z);
}

void Chunk::generateMeshForGeometry(const MeshGenContext& ctx) {
    for (auto f : ctx.geometry->faces()) {
        if (glm::abs(ctx.geometryState.angle) > std::numeric_limits<float>::epsilon() * 2) {
            f.rotate(ctx.geometryState.axis, ctx.geometryState.angle);
        }

        if (f.cull && !m_world->canSeeFace(*ctx.block, ctx.worldPos, f.cullDir))
            continue;

        f.translate(ctx.posInChunk);

        const TexID material = ctx.block->material().forTag(f.tag);
        for (Vertex v : f.vertices) {
            v.data(material, 0);
            ctx.storage.add(v);
        }

        if (f.doubleSided) [[unlikely]] {
            for (Vertex v : f.vertices | std::views::reverse) {
                v.data(material, 0);
                ctx.storage.add(v);
            }
        }
    }
}

void Chunk::generateMeshForBlock(
    const Block* block, glm::ivec3 pos, const BlockState* state, const glm::ivec3& chunkBlockCoords
) {
    gl::Attributes<Vertex>& storage = m_renderLayers[block->layer()].write();

    MeshGenContext ctx{
        .block = block,
        .geometry = block->geometry(),
        .geometryState = calculateGeometryState(block, state),
        .storage = storage,
        .posInChunk = pos,
        .worldPos = pos + chunkBlockCoords,
    };
    generateMeshForGeometry(ctx);
}

void Chunk::generateMeshForBlock(
    const VariantBlock* block,
    glm::ivec3 pos,
    const BlockState* state,
    const glm::ivec3& chunkBlockCoords
) {
    VariantBlock::Neighbours neighboringBlocks = getNeighbouringBlocks(pos);
    // TODO somehow distinguish between opaque and transparent geometries? or just use the base block layer?
    gl::Attributes<Vertex>& storage = m_renderLayers[block->layer()].write();

    GeometryState geoState = calculateGeometryState(block, state);
    if (geoState.angle != 0.0f) {
        neighboringBlocks.rotate(
            block->facingUp() ? Side::Up : Side::North, getSide(state->facing())
        );
    }

    for (auto geo : block->getGeometries(neighboringBlocks)) {
        MeshGenContext ctx{
            .block = block,
            .geometry = geo,
            .geometryState = geoState,
            .storage = storage,
            .posInChunk = pos,
            .worldPos = pos + chunkBlockCoords
        };
        generateMeshForGeometry(ctx);
    }
}

void Chunk::generateMeshForBlock(
    const MultiBlock* block,
    glm::ivec3 pos,
    const BlockState* state,
    const glm::ivec3& chunkBlockCoords
) {
    for (auto subBlock : block->blocks()) {
        generateMeshForBlock(
            RegistryManager::Blocks().get(subBlock.blockID),
            pos,
            subBlock.state.has_value() ? &subBlock.state.value() : nullptr,
            chunkBlockCoords
        );
    }
}

VariantBlock::Neighbours Chunk::getNeighbouringBlocks(glm::ivec3 pos) const {
    BlockState* northState = nullptr;
    BlockState* southState = nullptr;
    BlockState* eastState = nullptr;
    BlockState* westState = nullptr;
    BlockState* upState = nullptr;
    BlockState* downState = nullptr;

    glm::ivec3 chPos = position();
    return {
        .north = m_world->getBlockID(chPos + pos + INORTH, false, &northState),
        .south = m_world->getBlockID(chPos + pos - INORTH, false, &southState),
        .east = m_world->getBlockID(chPos + pos + IEAST, false, &eastState),
        .west = m_world->getBlockID(chPos + pos - IEAST, false, &westState),
        .up = m_world->getBlockID(chPos + pos + IUP, false, &upState),
        .down = m_world->getBlockID(chPos + pos - IUP, false, &downState),
        .northFacing = northState ? northState->facing() : glm::vec3(0.0f),
        .southFacing = southState ? southState->facing() : glm::vec3(0.0f),
        .eastFacing = eastState ? eastState->facing() : glm::vec3(0.0f),
        .westFacing = westState ? westState->facing() : glm::vec3(0.0f),
        .upFacing = upState ? upState->facing() : glm::vec3(0.0f),
        .downFacing = downState ? downState->facing() : glm::vec3(0.0f),
    };
}

void Chunk::serialize(std::ostream& out) const {
    m_data->serialize(out);
}

void Chunk::deserialize(std::istream& in) {
    m_data->deserialize(in);
}