#pragma once

#include <block/Geometry.h>
#include <glm/glm.hpp>

using namespace engine;

void RegisterGeometries();

Geometry CreateRotatedGeometry(Geometry geo, glm::vec3 axis, float angle);

Geometry Pyramid();
Geometry Log(int width);