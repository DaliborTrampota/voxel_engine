#include "TerrainGenerator.h"

int NoiseGenerator::getVoxel(glm::ivec3 pos)
{
	int height = static_cast<int>(noise.noise2D_01(pos.x * m_scale, pos.z * m_scale) * 5) + m_minHeight;

	if (pos.y < height)
		return 1;
    return 0;
}
