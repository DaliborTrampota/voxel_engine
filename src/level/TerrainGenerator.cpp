#include "TerrainGenerator.h"

#include "glm/gtc/random.hpp"
#include <block/Block.h>
#include <data/registry/Registry.h>

namespace data {
	extern Registry<Block> Blocks;
}

int NoiseGenerator::getVoxel(glm::ivec3 pos) const
{
	int height = getHeight(glm::ivec2(pos.x, pos.z));
	return getVoxel(pos, height);

}

int NoiseGenerator::getHeight(glm::ivec2 pos) const
{
	return static_cast<int>(noise.noise2D_01(pos.x * m_scale, pos.y * m_scale) * 5) + m_minHeight;
}

int NoiseGenerator::getVoxel(glm::ivec3 pos, int height) const
{
	int dirtHeight = static_cast<int>(noise.noise2D_01(pos.x * m_scale, pos.y * m_scale) * 2) + 3;
	if (pos.y == height + 1 && glm::linearRand(.0f, 1.0f) < 0.4f)
		return data::Blocks.get("pyramid").getID();

	if (pos.y > height)
		return 0;

	unsigned int blockID;
	if (pos.y == height) blockID = data::Blocks.get("grass").getID();
	else if (pos.y > dirtHeight) blockID = data::Blocks.get("dirt").getID();
	else blockID = data::Blocks.get("stone").getID();

	return blockID;
}


void NoiseGenerator::populate(std::vector<std::vector<std::vector<T>>> &data, glm::ivec3 posOffset)
{
	glm::ivec3 dims = glm::ivec3(data.size(), data[0].size(), data[0].at(0).size());
	for (int x = 0; x < dims.x; x++)
	{
		for (int z = 0; z < dims.z; z++)
		{
			int height = getHeight(glm::vec2(x + posOffset.x, z + posOffset.z));
			for (int y = 0; y < dims.y; y++)
			{
				data[x][y][z] = getVoxel(glm::ivec3(x, y, z) + posOffset, height);
			}
		}
	}
}
