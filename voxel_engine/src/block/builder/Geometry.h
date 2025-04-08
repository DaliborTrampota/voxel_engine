#pragma once

#include <vector>

#include "Face.h"
//#include "BlockTextures.h"


namespace builder {

	struct Geometry
	{
	//public:
		Geometry() = default;
		Geometry(std::vector<Face> faces);

		static Geometry Cube(const char* texName);
		static Geometry Cube(const char* texTop, const char* texSide);
		static Geometry Cube(const char* texTop, const char* texBottom, const char* texSide);
		static Geometry Cube(std::initializer_list<const char*> textures);

		static Geometry Cylinder(const char* texTop, const char* texBottom, const char* side);

	//private:
		std::vector<Face> m_faces;
	};
}