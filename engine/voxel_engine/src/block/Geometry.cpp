#include "Geometry.h"

#include "data/TextureManager.h"
#include "data/VertexData.h"


using namespace engine;

Geometry::Geometry() : m_faces() {
    m_id = s_idCounter++;
}

Geometry::Geometry(std::vector<Face> faces) : Geometry() {
    m_faces = std::move(faces);
}

Geometry Geometry::Cube() {
    Geometry g;
    g.m_faces.insert(g.m_faces.begin(), data::f_faces.begin(), data::f_faces.end());
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
