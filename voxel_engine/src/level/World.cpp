#include "World.h"

#include "TerrainGenerator.h"
#include "CoordUtils.h"
#include "block/Block.h"


using namespace lvl;

World::World(TerrainGenerator* gen) : generator(gen)
{
	Chunk* chunk = new Chunk(this, ChunkID(0, 0, 0));
	chunks[ChunkID(0, 0, 0)] = chunk;

	chunk->generateMesh();

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
}

lvl::World::~World()
{
}

bool lvl::World::checkBlock(glm::vec3 pos, data::Block& curBlock, glm::ivec3 dir) const
{
	ChunkID chunkCoords = util::extractChunkCoords(pos);
	if (!chunks.contains(chunkCoords))
		return false;
	
	Chunk* chunk = chunks.at(chunkCoords);
	data::Block block = chunk->getBlock(pos);



	if (!curBlock.isVoxel() && block.isVoxel() || curBlock.isVoxel() && !block.isVoxel()) {
		dir = -dir;
		for (const auto& f : block.geometry()->m_faces) {
			if (f.m_cullDir == dir)
				return true;
		}
		return false;
	}

	return block.getID() == curBlock.getID() || 
		block.isSolid() && curBlock.isSolid();
}

void lvl::World::render(ShaderPipeline* pipeline)
{
	for (auto& [pos, chunk] : chunks)
	{
		size_t vertices = chunk->prepareRender(pipeline);
		glDrawArrays(GL_TRIANGLES, 0, vertices);
	}
}
