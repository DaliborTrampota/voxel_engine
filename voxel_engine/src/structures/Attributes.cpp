#include "Attributes.h"

#include <glad/glad.h>

#include "ShaderPipeline.h"

#include "data/Vertex.h"


Attributes::Attributes(GLint drawType = GL_STATIC_DRAW) :
	m_drawType(drawType),
	m_elements(0)
{
    glGenBuffers(1, &m_buffer);
}

void Attributes::bind(ShaderPipeline* pipeline)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_data.size() * sizeof(vert::Vertex), m_data.data(), m_drawType);

	pipeline->registerAttribute(0, 3, GL_FLOAT, 9, 0);// offsetof(vert::Vertex, m_pos));
	pipeline->registerAttribute(1, 3, GL_FLOAT, 9, 3);// offsetof(vert::Vertex, m_normal));
	pipeline->registerAttribute(2, 2, GL_FLOAT, 9, 6);// offsetof(vert::Vertex, m_uv));
	pipeline->registerAttribute(3, 1, GL_INT, 9, 8);// offsetof(vert::Vertex, m_data));
}

