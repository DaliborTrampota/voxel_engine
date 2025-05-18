#include "Engine.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <gl/ShaderPipeline.h>
#include <level/Chunk.h>
#include <level/World.h>
#include <Camera.h>


using namespace engine;

void Engine::render(gl::ShaderPipeline* pipeline, Chunk *chunk)
{
	size_t verts = chunk->m_vertexData.length();
	if(verts == 0 || !chunk->m_generated)
		return;

	chunk->m_vertexData.bind();

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(chunk->m_coords * Chunk::Dims));
	pipeline->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, verts);
}

void Engine::render(gl::ShaderPipeline* pipeline, Camera* cam, std::shared_ptr<World> world)
{
    pipeline->setViewMatrix(cam->getView());
	for (auto& pos : world->m_loadedChunks)
	{
		render(pipeline, world->m_chunks[pos]);
	}
}
