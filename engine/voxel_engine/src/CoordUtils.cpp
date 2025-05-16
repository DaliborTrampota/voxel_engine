#include "CoordUtils.h"

namespace engine {


	ChunkID extractChunkCoords(glm::vec3& pos)
	{
		ChunkID chunkCoords = ChunkID(
			(int)floor(pos.x / ChunkDim.x),
			(int)floor(pos.y / ChunkDim.y),
			(int)floor(pos.z / ChunkDim.z)
		);

		pos.x -= chunkCoords.x * ChunkDim.x;
		pos.y -= chunkCoords.y * ChunkDim.y;
		pos.z -= chunkCoords.z * ChunkDim.z;

		return chunkCoords;
	}

}