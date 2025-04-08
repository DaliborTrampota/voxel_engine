#pragma once

#include <glm/glm.hpp>
#include "level/World.h"

namespace util
{

	/**
	* @param pos Position in world space
	* @return Chunk coordinates and the position of the block within the chunk is stored into pos
	*/
	lvl::ChunkID extractChunkCoords(glm::vec3& pos);

}

