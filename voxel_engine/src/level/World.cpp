#include "World.h"

#include "TerrainGenerator.h"
#include "CoordUtils.h"
#include "block/Block.h"

#include <mutex>
#include <queue>


using namespace lvl;

World::World(TerrainGenerator* gen) : m_generator(gen)
{
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
}

lvl::World::~World()
{
	for (auto& [pos, chunk] : m_chunks)
	{
		delete chunk;
	}
	//glDeleteVertexArrays(1, &m_VAO);
}

bool lvl::World::checkBlock(glm::vec3 pos, data::Block& curBlock, glm::ivec3 dir) const
{
	ChunkID chunkCoords = util::extractChunkCoords(pos);
	if (!m_chunks.contains(chunkCoords))
		return false;
	
	Chunk* chunk = m_chunks.at(chunkCoords);
	if (!chunk)
		return false;
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

void lvl::World::updateViewDistance(glm::vec3 pos)
{
	std::queue<Chunk*> toLoad;

	ChunkID chunkCoords = util::extractChunkCoords(pos);
	int viewDistance = 5;
	int yViewDistance = 3;
	for (int x = chunkCoords.x - viewDistance; x <= chunkCoords.x + viewDistance; ++x)
	{
		for (int y = chunkCoords.y - yViewDistance; y <= chunkCoords.y + yViewDistance; ++y)
		{
			for (int z = chunkCoords.z - viewDistance; z <= chunkCoords.z + viewDistance; ++z)
			{
				ChunkID newCoords = ChunkID(x, y, z);
				if (m_loadedChunks.contains(newCoords))
				{
					//chunk->setRender(true)
				} else if (!m_chunks.contains(newCoords))
				{
					Chunk* chunk = new Chunk(this, newCoords);
					m_chunks[newCoords] = chunk;
					toLoad.push(chunk);
				}
			}
		}
	}

	std::mutex genMutex;
	auto generateChunk = [this, &toLoad, &genMutex]() {

		while (true)
		{
			Chunk* chunk = nullptr;
			{
				std::lock_guard<std::mutex> lock(genMutex);
				if (toLoad.empty()) return;
				chunk = toLoad.front();
				toLoad.pop();
			}
			if (chunk) {
				chunk->populate();
				m_loadedChunks.insert(chunk->getID());
			}
		}
	};

	std::thread gen1(generateChunk);
	std::thread gen2(generateChunk);

	gen1.join();
	gen2.join();
}

void lvl::World::render(ShaderPipeline* pipeline)
{
	//for (auto& [pos, chunk] : m_chunks)
	for (auto& pos : m_loadedChunks)
	{
		size_t vertices = m_chunks[pos]->prepareRender(pipeline);
		if (!vertices)
			continue;
		glDrawArrays(GL_TRIANGLES, 0, vertices);
	}
}
