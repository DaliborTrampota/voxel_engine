#include "Chunk.h"

#include <glad/glad.h>	
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Structures/ShaderPipeline.h"
#include "World.h";
#include "data/VertexData.h"

using namespace lvl;

lvl::Chunk::Chunk(World* world, ChunkID coords) :
	m_world(world),
	m_coords(coords),
	m_vertexData(GL_DYNAMIC_DRAW)
{
	populate();
	generateMesh();
}

void Chunk::populate()
{
	glm::ivec3 posOffset = m_coords * ChunkDim;

	for (int x = 0; x < ChunkDim.x; x++)
	{
		m_data.push_back(std::vector<std::vector<T>>());
		for (int y = 0; y < ChunkDim.y; y++)
		{
			m_data[x].push_back(std::vector<T>());
			for (int z = 0; z < ChunkDim.z; z++)
			{
				glm::ivec3 pos(x, y, z);
				int blockID = m_world->generator->getVoxel(pos + posOffset);
				m_data[x][y].push_back(blockID);
			}
		}
	}
}

void Chunk::generateMesh()
{

	for (int x = 0; x < ChunkDim.x; x++)
	{
		m_vertexData.reserve(ChunkDim.x * ChunkDim.y * ChunkDim.z * 6); // 16x16x6 faces (6 vertices per face

		for (int y = 0; y < ChunkDim.y; y++)
		{
			for (int z = 0; z < ChunkDim.z; z++)
			{
				if(m_data[x][y][z] == 0)
					continue;

				glm::ivec3 pos(x, y, z);
				int blockID = m_data[x][y][z];

				for (int side = 0; side < 6; ++side) 
				{
					for (int i = 0; i < 6; ++i) 
					{
						m_vertexData.add(
							vert::vertices[vert::faces[side][i]] + (glm::vec3)pos,
							vert::normals[side],
							vert::uvs[i],
							blockID,
							0
						);
					}
				}
			}
		}
	}		

	m_vertexData.m_data.shrink_to_fit();
}

GLsizei lvl::Chunk::prepareRender(ShaderPipeline* pipeline)
{
	m_vertexData.bind(pipeline);

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(m_coords * ChunkDim));
	pipeline->setMat4("model", model);

	return m_vertexData.length();
}

bool lvl::ChunkID::operator()(const ChunkID& a, const ChunkID& b) const
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}
