
#include "Vertex.h"

using namespace engine;

Vertex::Vertex(glm::vec3 pos, glm::vec3 n, glm::vec2 uv, int textureID, int ao)
    : m_pos(pos),
      m_normal(n),
      m_uv(uv) {
    //m_data = pos.x << 16 | pos.y << 8 | pos.z;
    //m_data = n.x << 3 | n.y
    setData(textureID, ao);
}

Vertex::Vertex(glm::vec3 pos, glm::vec3 n, glm::vec2 uv) : Vertex::Vertex(pos, n, uv, 0, 0) {}

void Vertex::translate(glm::vec3 t) {
    m_pos += t;
}

void Vertex::setData(int textureID, int ao) {
    m_data = textureID | ao << 10u;  // textureID is 0-10, ao is 11-12,
}
