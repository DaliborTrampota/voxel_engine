
#include "Vertex.h"

#include <utility/Algorithms.h>

using namespace engine;

Vertex::Vertex(glm::vec3 pos, glm::vec3 n, glm::vec2 uv, int textureID, int ao)
    : pos(pos),
      normal(n),
      uv(uv) {
    //m_data = pos.x << 16 | pos.y << 8 | pos.z;
    //m_data = n.x << 3 | n.y
    data(textureID, ao);
}

Vertex::Vertex(glm::vec3 pos, glm::vec3 n, glm::vec2 uv) : Vertex::Vertex(pos, n, uv, 0, 0) {}

void Vertex::translate(glm::vec3 t) {
    pos += t;
}

void Vertex::rotate(glm::vec3 axis, float angle) {
    pos = rotatePoint(pos, axis, angle);
    normal = rotatePoint(normal, axis, angle, {0, 0, 0});
}

void Vertex::data(int textureID, int ao) {
    m_data = textureID | ao << 16u;  // textureID is 0-15, ao is 16-17,
}
