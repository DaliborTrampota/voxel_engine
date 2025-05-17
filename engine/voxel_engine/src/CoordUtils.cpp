#include "CoordUtils.h"

namespace engine {


	ChunkID extractChunkCoords(glm::vec3& pos)
	{
		ChunkID chunkCoords = ChunkID(
			(int)floor(pos.x / Chunk::Dims.x),
			(int)floor(pos.y / Chunk::Dims.y),
			(int)floor(pos.z / Chunk::Dims.z)
		);

		pos.x -= chunkCoords.x * Chunk::Dims.x;
		pos.y -= chunkCoords.y * Chunk::Dims.y;
		pos.z -= chunkCoords.z * Chunk::Dims.z;

		return chunkCoords;
	}

}