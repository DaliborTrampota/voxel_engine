#include "World.h"

#include "TerrainGenerator.h"
#include "CoordUtils.h"
#include "block/Block.h"
#include "block/Geometry.h"

#include <mutex>
#include <queue>

#include <iostream>

using namespace engine;

World::World(TerrainGenerator* gen) : m_generator(gen)
{
	m_genPool = new ThreadPool<>(8);
}

World::~World()
{
	if (m_genPool)
		m_genPool->stop();

	for (auto& [pos, chunk] : m_chunks)
	{
		delete chunk;
	}
}

bool World::checkBlock(glm::vec3 pos, Block& curBlock, glm::ivec3 dir) const
{
	ChunkID chunkCoords = engine::extractChunkCoords(pos);
	// std::cout << "chunkCoords = " << chunkCoords.x << ", " << chunkCoords.y << ", " << chunkCoords.z << "\n";
	if (!m_chunks.contains(chunkCoords))
		return false;
	
	Chunk* chunk = m_chunks.at(chunkCoords);
	if (!chunk)
		return false;
	Block block = chunk->getBlock(pos);



	if (!curBlock.isVoxel() && block.isVoxel() || curBlock.isVoxel() && !block.isVoxel()) {
		dir = -dir;
		for (const auto& f : block.geometry()->faces()) {
			if (f.m_cullDir == dir)
				return true;
		}
		return false;
	}

	return block.getID() == curBlock.getID() || 
		block.isSolid() && curBlock.isSolid();
}

void World::updateViewDistance(glm::vec3 pos)
{

	ChunkID chunkCoords = engine::extractChunkCoords(pos);
	int viewDistance = 4;
	int yViewDistance = 3;

	m_genPool->pause();

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
				} 
                else if (!m_chunks.contains(newCoords)) {

					Chunk* chunk = new Chunk(this, newCoords);
					m_chunks[newCoords] = chunk;

					m_genPool->add([this, chunk] {
						chunk->populate();
						m_loadedChunks.insert(chunk->getID());
					});
				}
			}
		}
	}

	m_genPool->resume();
}