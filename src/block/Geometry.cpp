#include "Geometry.h"

#include "data/VertexData.h"

#include "../block/Side.h"


using namespace engine;

Geometry::Geometry() : m_faces() {
    m_id = s_idCounter++;
}

Geometry::Geometry(std::vector<Face> faces) : m_faces(std::move(faces)) {
    m_id = s_idCounter++;
    m_aabb = AABB::fromGeometry(*this);
}

Geometry::Geometry(std::vector<Face> faces, const AABB& aabb)
    : m_faces(std::move(faces)),
      m_aabb(aabb) {
    m_id = s_idCounter++;
}

void Geometry::rotate(glm::vec3 axis, float angle) {
    for (auto& face : m_faces) {
        face.rotate(axis, angle);
    }
    m_aabb = AABB::fromGeometry(*this);
}

Geometry Geometry::Cube() {
    Geometry g;
    g.m_faces.insert(g.m_faces.begin(), data::f_faces.begin(), data::f_faces.end());
    g.m_aabb = AABB::fromGeometry(g);
    return g;
}


Geometry Geometry::Cylinder() {
    Geometry g;
    g.m_faces.push_back(
        Face::CircleFace(
            FaceTag::Top,
            glm::vec3(.5f, 1, .5f),
            0.5f,
            5,
            glm::vec3(0, 1, 0),
            glm::vec2{0, 0},
            glm::vec2{1, 1}
        )
    );


    g.m_faces.push_back(
        Face::CircleFace(
            FaceTag::Bottom,
            glm::vec3(.5f, 0, .5f),
            0.5f,
            5,
            glm::vec3(0, -1, 0),
            glm::vec2{0, 0},
            glm::vec2{1, 1}
        )
    );


    g.m_faces.push_back(
        Face::CylinderFace(
            FaceTag::Side,
            glm::vec3(.5f, 0, .5f),
            0.5f,
            1,
            5,
            glm::vec3(0, 1, 0),
            glm::vec2{0, 0},
            glm::vec2{1, 1}
        )
    );

    return g;
}

Geometry Geometry::Box(glm::vec3 start, glm::vec3 end) {
    Geometry g;

    for (int i = 0; i < 6; i++) {
        Face face;
        face.tag = static_cast<FaceTag>(i + 2);
        int uvIdx = 0;
        for (int vertIdx : data::faces[i]) {
            glm::vec3 vert = data::vertices[vertIdx];

            assert(FaceTag::Front == static_cast<FaceTag>(static_cast<int>(Side::North) + 2));

            vert.x = vert.x * (end.x - start.x) + start.x;
            vert.y = vert.y * (end.y - start.y) + start.y;
            vert.z = vert.z * (end.z - start.z) + start.z;

            face.vertices.push_back({vert, data::normals[i], data::uvs[uvIdx]});
            uvIdx++;
        }
        g.m_faces.push_back(face);
    }

    g.m_aabb = AABB::fromGeometry(g);
    return g;
}
