#include "CoordUtils.h"

namespace util {


	lvl::ChunkID extractChunkCoords(glm::vec3& pos)
	{
		lvl::ChunkID chunkCoords = lvl::ChunkID(
			(int)floor(pos.x / lvl::ChunkDim.x),
			(int)floor(pos.y / lvl::ChunkDim.y),
			(int)floor(pos.z / lvl::ChunkDim.z)
		);

		pos.x -= chunkCoords.x * lvl::ChunkDim.x;
		pos.y -= chunkCoords.y * lvl::ChunkDim.y;
		pos.z -= chunkCoords.z * lvl::ChunkDim.z;

		return chunkCoords;
	}

}