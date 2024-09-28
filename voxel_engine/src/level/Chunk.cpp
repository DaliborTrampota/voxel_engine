#include "Chunk.h"

#include <glad/glad.h>	
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "structures/Attributes.h"
#include "structures/ShaderPipeline.h"

#include "data/VertexData.h"
#include "data/Vertex.h"

#include "World.h";
#include "TerrainGenerator.h"

#include "block/Block.h"
//#include "block/builder/CulledGeometry.h"
//#include "block/builder/CubeGeometry.h"

#include "data/registry/Registry.h"

namespace data {
	extern Registry<data::Block> Blocks;

}


using namespace lvl;

lvl::Chunk::Chunk(World* world, ChunkID coords) :
	m_world(world),
	m_coords(coords),
	m_vertexData(GL_DYNAMIC_DRAW)
{
	//populate();
}

lvl::Chunk::~Chunk()
{
	if (m_genThread.joinable())
		m_genThread.join();
}

void Chunk::generate()
{
	m_genThread = std::thread(&Chunk::populate, this);
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

				data::Block block = data::Blocks.get(blockID);

				for (auto f : block.geometry()->m_faces) {

					if (f.m_cull && m_world->checkBlock(pos + f.m_cullDir + chunkBlockCoords, block, f.m_cullDir))
						continue;

					f.translate(pos);
					m_vertexData.addFace(f);
				}
			}
		}
	}		

	m_vertexData.m_data.shrink_to_fit();
	m_generated = true;
}

data::Block lvl::Chunk::getBlock(glm::ivec3 pos) const
{
	T blockID = m_data.size() == 0 
		? m_world->m_generator->getVoxel(pos + m_coords * ChunkDim) 
		: m_data[pos.x][pos.y][pos.z];
	return data::Blocks.get(blockID);
}

GLsizei lvl::Chunk::prepareRender(ShaderPipeline* pipeline)
{
	if(m_vertexData.length() == 0 || !m_generated)
		return 0;

	m_vertexData.bind(pipeline);

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(m_coords * ChunkDim));
	pipeline->setMat4("model", model);

	return m_vertexData.length();
}

bool lvl::ChunkID::operator()(const ChunkID& a, const ChunkID& b) const
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

