#include "World.h"

#include "TerrainGenerator.h"

using namespace lvl;

World::World(TerrainGenerator* gen) : generator(gen)
{
	Chunk* chunk = new Chunk(this, ChunkID(0, 0, 0));
	chunks[ChunkID(0, 0, 0)] = chunk;

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
}

lvl::World::~World()
{
}

void lvl::World::render(ShaderPipeline* pipeline)
{
	for (auto& [pos, chunk] : chunks)
	{
		size_t triangles = chunk->prepareRender(pipeline);
		glDrawArrays(GL_TRIANGLES, 0, triangles);
	}
}
