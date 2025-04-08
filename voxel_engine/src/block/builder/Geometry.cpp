#include "Geometry.h"

#include "data/VertexData.h"
#include "data/TextureManager.h"


namespace data {
	extern TextureManager textureManager;
}


builder::Geometry::Geometry(std::vector<Face> faces) : m_faces(faces)
{
}

builder::Geometry builder::Geometry::Cube(const char* texName)
{
	Geometry g;

	g.m_faces.insert(g.m_faces.begin(), data::f_faces.begin(), data::f_faces.end());

	for (auto& f : g.m_faces)
	{
		f.setData(data::textureManager.get(texName), 0);
	}


	return g;
}

builder::Geometry builder::Geometry::Cube(const char* texTop, const char* texSide)
{
	return Geometry::Cube(texTop, texTop, texSide);
}

builder::Geometry builder::Geometry::Cube(const char* texTop, const char* texBottom, const char* texSide)
{
	return Geometry::Cube({ texSide, texSide, texSide, texSide, texTop, texBottom });
	Geometry g;

	g.m_faces.insert(g.m_faces.begin(), data::f_faces.begin(), data::f_faces.end());

	g.m_faces[0].setData(data::textureManager.get(texSide), 0);
	g.m_faces[1].setData(data::textureManager.get(texSide), 0);
	g.m_faces[2].setData(data::textureManager.get(texSide), 0);
	g.m_faces[3].setData(data::textureManager.get(texSide), 0);

	g.m_faces[4].setData(data::textureManager.get(texTop), 0);
	g.m_faces[5].setData(data::textureManager.get(texBottom), 0);

	return g;
}

builder::Geometry builder::Geometry::Cube(std::initializer_list<const char*> textures)
{
	Geometry g;
	g.m_faces.insert(g.m_faces.begin(), data::f_faces.begin(), data::f_faces.end());

	int i = 0;
	for (auto tex : textures)
		g.m_faces[i++].setData(data::textureManager.get(tex), 0);
	return g;
}

builder::Geometry builder::Geometry::Cylinder(const char* texTop, const char* texBottom, const char* side)
{
	Geometry g;
	g.m_faces.push_back(Face::CircleFace(
		glm::vec3(.5f, 1, .5f),
		0.5f,
		5,
		glm::vec3(0, 1, 0),
		glm::vec2{ 0, 0 },
		glm::vec2{ 1, 1 },
		data::textureManager.get(texTop)
	));


	g.m_faces.push_back(Face::CircleFace(
		glm::vec3(.5f, 0, .5f),
		0.5f,
		5,
		glm::vec3(0, -1, 0),
		glm::vec2{ 0, 0 },
		glm::vec2{ 1, 1 },
		data::textureManager.get(texBottom)
	));


	g.m_faces.push_back(Face::CylinderFace(
		glm::vec3(.5f, 0, .5f),
		0.5f,
		1,
		5,
		glm::vec3(0, 1, 0),
		glm::vec2{ 0, 0 },
		glm::vec2{ 1, 1 },
		data::textureManager.get(side)
	));

	return g;
}

