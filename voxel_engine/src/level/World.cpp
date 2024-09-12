#include "World.h"

using namespace lvl;

World::World(TerrainGenerator* gen) : generator(gen)
{
	Chunk* chunk = new Chunk(this, glm::ivec2(0, 0));
	chunks[glm::ivec2(0, 0)] = chunk;

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
}

lvl::World::~World()
{
	delete generator;
}

void lvl::World::render(ShaderPipeline* pipeline)
{
	for (auto& [pos, chunk] : chunks)
	{
		size_t triangles = chunk->prepareRender(pipeline);
		glDrawArrays(GL_TRIANGLES, 0, triangles);
	}
}
