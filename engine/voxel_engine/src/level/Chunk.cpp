#include "Chunk.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <gl/Attributes.h>
#include <gl/ShaderPipeline.h>

#include "data/VertexData.h"
#include "block/Vertex.h"

#include "World.h";
#include "TerrainGenerator.h"

#include "block/Block.h"
//#include "block/builder/CulledGeometry.h"
//#include "block/builder/CubeGeometry.h"

#include "data/RegistryManager.h"

#include <Globals.h>


using namespace engine;

Chunk::Chunk(World* world, ChunkID coords) :
	m_world(world),
	m_coords(coords),
	m_vertexData(GL_DYNAMIC_DRAW)
{
	GL_GUARD;
	m_vertexData.create();
	//populate();
}

Chunk::~Chunk()
{
	if (m_genThread.joinable())
		m_genThread.join();
}

void Chunk::generate()
{
	//m_genThread = std::thread(&Chunk::populate, this);
	//m_genThread.join();

}

void Chunk::populate()
{
	glm::ivec3 posOffset = m_coords * ChunkDim;
	m_data = VoxelData(ChunkDim.x, std::vector<std::vector<T>>(ChunkDim.y, std::vector<T>(ChunkDim.z, 0)));
	m_world->m_generator->populate(m_data, posOffset);
	generateMesh();
}

void Chunk::generateMesh()
{
	glm::ivec3 chunkBlockCoords = m_coords * ChunkDim;
	for (int x = 0; x < ChunkDim.x; x++)
	{
		m_vertexData.reserve(ChunkDim.x * ChunkDim.y * ChunkDim.z * 6); // 16x16x6 faces (6 vertices per face)

		for (int y = 0; y < ChunkDim.y; y++)
		{
			for (int z = 0; z < ChunkDim.z; z++)
			{
				if(m_data[x][y][z] == 0)
					continue;

				glm::ivec3 pos(x, y, z);
				int blockID = m_data[x][y][z];

				Block block = RegistryManager::Blocks().get(blockID);

				for (auto f : block.geometry()->faces()) {
					if (f.m_cull && m_world->checkBlock(pos + f.m_cullDir + chunkBlockCoords, block, f.m_cullDir))
						continue;

					f.translate(pos);

					for (Vertex v : f.m_vertices) {
                        v.setData(block.material().forTag(f.tag), 0);
						m_vertexData.add(v);
					}
				}
			}
		}
	}		

	m_vertexData.m_data.shrink_to_fit();
	m_generated = true;
}

Block Chunk::getBlock(glm::ivec3 pos) const
{
	T blockID = m_data.size() == 0 
		? m_world->m_generator->getVoxel(pos + m_coords * ChunkDim) 
		: m_data[pos.x][pos.y][pos.z];
	return RegistryManager::Blocks().get(blockID);
}

bool ChunkID::operator()(const ChunkID& a, const ChunkID& b) const
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

bool ChunkID::operator==(const ChunkID& other) const
{
	return x == other.x && y == other.y && z == other.z;
}

