#include "Chunk.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <LWGL/buffer/Attributes.h>

#include "World.h"
#include "block/Block.h"
#include "block/Vertex.h"
#include "data/RegistryManager.h"
#include "render/Engine.h"
#include "render/RenderContext.h"
//#include "block/builder/CulledGeometry.h"
//#include "block/builder/CubeGeometry.h"


using namespace engine;

Chunk::Chunk(World* world, ChunkID coords)
    : m_world(world),
      m_coords(coords),
      m_data(Chunk::Dims),
      m_opaqueVertData(GL_DYNAMIC_DRAW),
      m_transparentVertData(GL_DYNAMIC_DRAW) {
    m_opaqueVertData.create();
    m_transparentVertData.create();
}

Chunk::~Chunk() {}

void Chunk::generate() {
    if (m_generated)
        return;

    m_world->m_generator->populate(*this);
    m_data.populated = true;
}

bool Chunk::generateMesh() {
    bool expected = false;
    if (!m_generatingMesh.compare_exchange_strong(expected, true))
        return false;

    glm::ivec3 chunkBlockCoords = m_coords * Chunk::Dims;
    m_opaqueVertData.clear();
    m_transparentVertData.clear();

    m_opaqueVertData.reserve(
        Chunk::Dims.x * Chunk::Dims.y * Chunk::Dims.z * 6
    );  // 16x16x6 faces (6 vertices per face)
    m_transparentVertData.reserve(
        Chunk::Dims.x * Chunk::Dims.y * 6
    );  // 16x16x6 faces (6 vertices per face)

    for (int x = 0; x < Chunk::Dims.x; x++) {
        for (int y = 0; y < Chunk::Dims.y; y++) {
            for (int z = 0; z < Chunk::Dims.z; z++) {
                if (m_data(x, y, z) == 0)
                    continue;

                glm::ivec3 pos(x, y, z);

                BlockID blockID = m_data.getBlock(pos);
                Block block = RegistryManager::Blocks().get(blockID);

                gl::Attributes<Vertex>& storage =
                    block.layer() == Layers::Opaque ? m_opaqueVertData : m_transparentVertData;

                for (auto f : block.geometry()->faces()) {
                    if (f.cull && !m_world->canSeeFace(block, pos + chunkBlockCoords, f.cullDir))
                        continue;

                    f.translate(pos);

                    for (Vertex v : f.vertices) {
                        v.data(block.material().forTag(f.tag), 0);
                        storage.add(v);
                    }
                }
            }
        }
    }
    m_generatingMesh = false;
    m_opaqueVertData.vertexData().shrink_to_fit();
    m_transparentVertData.vertexData().shrink_to_fit();
    m_generated = true;
    return true;
}

const Block& Chunk::getBlock(glm::ivec3 pos) const {
    BlockID blockID = !m_data.populated
                          ? m_world->m_generator->voxelAt(pos + m_coords * Chunk::Dims)
                          : m_data.getBlock(pos);
    return RegistryManager::Blocks().get(blockID);
}


void Chunk::render(Engine& engine, const Camera* camera, int pass) {
    if (m_dirty) {
        generateMesh();
        m_dirty = false;
    }
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

bool ChunkID::operator==(const ChunkID& other) const {
    return x == other.x && y == other.y && z == other.z;
}
