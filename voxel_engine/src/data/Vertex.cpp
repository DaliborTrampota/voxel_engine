#include "Vertex.h"

#pragma once

vert::Vertex::Vertex(glm::vec3 pos, glm::vec3 n, glm::vec2 uv, int textureID, int ao) :
	m_pos(pos),
	m_normal(n),
	m_uv(uv)
{
	//m_data = pos.x << 16 | pos.y << 8 | pos.z;
	//m_data = n.x << 3 | n.y
	m_data = textureID | ao << 10u; // textureID is 0-10, ao is 11-12, 
}
