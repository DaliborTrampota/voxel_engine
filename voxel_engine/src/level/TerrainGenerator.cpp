#include "TerrainGenerator.h"


int NoiseGenerator::getVoxel(glm::ivec3 pos)
{
	int height = static_cast<int>(noise.noise2D_01(pos.x, pos.y)) * 64 + 20;

	if (pos.y < height)
		return 1;
    return 0;
}
