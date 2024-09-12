#include "Attribute.h"

#include <glad/glad.h>


#include "ShaderPipeline.h"

Attribute::Attribute(int size, GLint drawType = GL_STATIC_DRAW) :
    m_size(size),
	m_drawType(drawType),
	m_elements(0)
{
    glGenBuffers(1, &m_buffer);
}

void Attribute::bind(ShaderPipeline* pipeline, int location)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_data.size() * sizeof(float), m_data.data(), m_drawType);
	pipeline->registerAttribute(location, m_size, GL_FLOAT, 0);
}

//void Attribute::add(float... data)
//{
//	m_data.push_back(data);
//	m_elements++;
//}


