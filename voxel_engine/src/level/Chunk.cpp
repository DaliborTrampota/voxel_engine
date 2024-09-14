#include "Chunk.h"

#include <glad/glad.h>	

#include "World.h";
#include "../VertexData.h"
#include "Structures/ShaderPipeline.h"

using namespace lvl;

lvl::Chunk::Chunk(World* world, ChunkID coords) :
	m_world(world),
	m_coords(coords),
	m_vertexData(3, GL_DYNAMIC_DRAW)
{
	populate();
	generateMesh();
}

void Chunk::populate()
{
	for (int x = 0; x < 16; x++)
	{
		m_data.push_back(std::vector<std::vector<T>>());
		for (int y = 0; y < 16; y++)
		{
			m_data[x].push_back(std::vector<T>());
			for (int z = 0; z < 16; z++)
			{
				glm::ivec3 pos(x, y, z);
				int blockID = m_world->generator->getVoxel(pos);
				m_data[x][y].push_back(blockID);
			}
		}
	}
}

void lvl::Chunk::generateMesh()
{

	for (int x = 0; x < 16; x++)
	{
		if(m_vertexData.m_data.capacity() - m_vertexData.m_data.size() < 100)
			m_vertexData.m_data.reserve(16 * 16 * 6);

		for (int y = 0; y < 16; y++)
		{
			for (int z = 0; z < 16; z++)
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
							vert::vertices[vert::faces[side][i]][0] + x,
							vert::vertices[vert::faces[side][i]][1] + y,
							vert::vertices[vert::faces[side][i]][2] + z,

							vert::normals[side][0],
							vert::normals[side][1],
							vert::normals[side][2]
						);
					}
				}
			}
		}
	}		

	m_vertexData.m_data.shrink_to_fit();
}

size_t lvl::Chunk::prepareRender(ShaderPipeline* pipeline)
{
	m_vertexData.bind(pipeline, 0);
	pipeline->registerAttribute(0, m_vertexData.size(), GL_FLOAT, 6);
	pipeline->registerAttribute(1, m_vertexData.size(), GL_FLOAT, 6, 3);
	return m_vertexData.length();
}

bool lvl::ChunkID::operator()(const ChunkID& a, const ChunkID& b) const
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}
