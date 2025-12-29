#include "Chunk.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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

Chunk::Chunk(World* world, ChunkID coords)
    : m_world(world),
      m_coords(coords),
      m_data(Chunk::Dims),
      m_opaqueVertData(GL_DYNAMIC_DRAW),
      m_transparentVertData(GL_DYNAMIC_DRAW),
      m_backOpaqueVertData(GL_DYNAMIC_DRAW),
      m_backTransparentVertData(GL_DYNAMIC_DRAW) {
    m_opaqueVertData.create();
    m_transparentVertData.create();
    m_backOpaqueVertData.create();
    m_backTransparentVertData.create();
}

Chunk::~Chunk() {}

void Chunk::generate() {
    if (m_generated || m_data.populated)
        return;

    m_world->m_generator->populate(*this);
    m_data.populated = true;
}

bool Chunk::generateMesh() {
    bool expected = false;
    if (!m_generatingMesh.compare_exchange_strong(expected, true))
        return false;

    glm::ivec3 chunkBlockCoords = m_coords * Chunk::Dims;
    m_backOpaqueVertData.clear();
    m_backTransparentVertData.clear();

    m_backOpaqueVertData.reserve(
        Chunk::Dims.x * Chunk::Dims.y * 8 * 6
    );  // 16x16x8x6 faces (6 vertices per face) the 8 is taking into account height variation
    m_backTransparentVertData.reserve(
        Chunk::Dims.x * Chunk::Dims.y * 6
    );  // 16x16x6 faces (6 vertices per face) less transparent blocks

    for (int x = 0; x < Chunk::Dims.x; x++) {
        for (int y = 0; y < Chunk::Dims.y; y++) {
            for (int z = 0; z < Chunk::Dims.z; z++) {
                if (m_data.getBlock({x, y, z}) == Block::AirID)
                    continue;

                glm::ivec3 pos(x, y, z);

                BlockID blockID = m_data.getBlock(pos);
                const Block* block = RegistryManager::Blocks().get(blockID);

                if (!block) {
                    // Block not found in registry, skip, TODO render pink/black block missing texture?
                    continue;
                }

                const BlockState* state = m_data.getState(pos);

                if (auto variant = dynamic_cast<const VariantBlock*>(block)) {
                    generateMeshForBlock(variant, pos, state, chunkBlockCoords);
                } else if (block->isMultiblock()) {
                    MultiBlock* multi = m_data.getMultiBlock(pos);
                    generateMeshForBlock(multi, pos, state, chunkBlockCoords);
                } else {
                    generateMeshForBlock(block, pos, state, chunkBlockCoords);
                }
            }
        }
    }
    m_generatingMesh = false;

    m_backOpaqueVertData.vertexData().shrink_to_fit();
    m_backTransparentVertData.vertexData().shrink_to_fit();
    m_opaqueVertData = std::move(m_backOpaqueVertData);
    m_transparentVertData = std::move(m_backTransparentVertData);

    m_generated = true;
    return true;
}

const Block* Chunk::getBlock(glm::ivec3 pos) const {
    BlockID blockID = !m_data.populated
                          ? m_world->m_generator->voxelAt(pos + m_coords * Chunk::Dims)
                          : m_data.getBlock(pos);
    return RegistryManager::Blocks().get(blockID);
}

const Block* Chunk::getBlock(glm::ivec3 pos, const BlockState*& state) const {
    state = nullptr;
    BlockID blockID = !m_data.populated
                          ? m_world->m_generator->voxelAt(pos + m_coords * Chunk::Dims)
                          : m_data.getBlockAndState(pos, state);
    return RegistryManager::Blocks().get(blockID);
}


void Chunk::render(Engine& engine, const Camera* camera, int pass) {
    if (!m_generated) {
        return;
    }

    RenderContext ctx;
    ctx.setModelMatrix(m_coords * Chunk::Dims);
    if (pass == 0) {
        ctx.attributes = &m_opaqueVertData;
        ctx.material = &m_world->m_material;
        ctx.passMask = RenderPass::Scene | RenderPass::DirectionalShadow;
        ctx.camera = camera;
        engine.submitRender(std::move(ctx));

        RenderContext ctxTransparent;
        ctxTransparent.setModelMatrix(m_coords * Chunk::Dims);
        ctxTransparent.attributes = &m_transparentVertData;
        ctxTransparent.material = &m_world->m_material;
        ctxTransparent.passMask = RenderPass::SceneTransparent | RenderPass::DirectionalShadow;
        ctxTransparent.camera = camera;
        engine.submitRender(std::move(ctxTransparent));
    }

    else if (pass == 1) {  // Opaque front to back
        if (!m_opaqueVertData.length())
            return;
        ctx.attributes = &m_opaqueVertData;
        ctx.material = &m_world->m_material;
        ctx.passMask = RenderPass::Scene | RenderPass::DirectionalShadow;
        ctx.camera = camera;
        engine.submitRender(std::move(ctx));
    }

    else if (pass == 2) {  // Transparent back to front
        if (!m_transparentVertData.length())
            return;
        ctx.attributes = &m_transparentVertData;
        ctx.material = &m_world->m_material;
        ctx.passMask = RenderPass::SceneTransparent | RenderPass::DirectionalShadow;
        ctx.camera = camera;
        engine.submitRender(std::move(ctx));
    }
}

// TODO get rid of this now when there is calculateRotationFromState
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
    gl::Attributes<Vertex>& storage =
        block->layer() == Layers::Opaque ? m_backOpaqueVertData : m_backTransparentVertData;

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
    gl::Attributes<Vertex>& storage =
        block->layer() == Layers::Opaque ? m_backOpaqueVertData : m_backTransparentVertData;

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
        .north = m_world->getBlockID(chPos + pos + INORTH, northState, false),
        .south = m_world->getBlockID(chPos + pos - INORTH, southState, false),
        .east = m_world->getBlockID(chPos + pos + IEAST, eastState, false),
        .west = m_world->getBlockID(chPos + pos - IEAST, westState, false),
        .up = m_world->getBlockID(chPos + pos + IUP, upState, false),
        .down = m_world->getBlockID(chPos + pos - IUP, downState, false),
        .northFacing = northState ? northState->facing() : glm::vec3(0.0f),
        .southFacing = southState ? southState->facing() : glm::vec3(0.0f),
        .eastFacing = eastState ? eastState->facing() : glm::vec3(0.0f),
        .westFacing = westState ? westState->facing() : glm::vec3(0.0f),
        .upFacing = upState ? upState->facing() : glm::vec3(0.0f),
        .downFacing = downState ? downState->facing() : glm::vec3(0.0f),
    };
}