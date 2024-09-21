#include "TerrainGenerator.h"

#include "glm/gtc/random.hpp"
#include "data/registry/Registry.h"
#include "block/Block.h"

namespace data {
	extern Registry<Block> Blocks;
}

int NoiseGenerator::getVoxel(glm::ivec3 pos)
{
	int height = getHeight(glm::ivec2(pos.x, pos.z));

	//if (pos.y == 10 && pos.x == 10 && pos.z == 10)
	//	return 1;
	//return 0;
	if (pos.y == height)
		return 1;
	if (pos.y < height)
		return 2;
	return 0;

}

int NoiseGenerator::getHeight(glm::ivec2 pos)
{
	return static_cast<int>(noise.noise2D_01(pos.x * m_scale, pos.y * m_scale) * 5) + m_minHeight;
}


void NoiseGenerator::populate(std::vector<std::vector<std::vector<T>>> &data, glm::ivec3 posOffset)
{
	glm::ivec3 dims = glm::ivec3(data.size(), data[0].size(), data[0].at(0).size());
	for (int x = 0; x < dims.x; x++)
	{
		for (int z = 0; z < dims.z; z++)
		{
			int height = getHeight(glm::vec2(x + posOffset.x, z + posOffset.z));
			int dirtHeight = height - glm::linearRand(3, 5);

			for (int y = 0; y < dims.y; y++)
			{
				if (y == height + 1 && glm::linearRand(.0f, 1.0f) < 0.4f)
					data[x][y][z] = data::Blocks.get("pyramid").getID();

				if (y > height)
					continue;

				unsigned int blockID;
				if(y == height) blockID = data::Blocks.get("grass").getID();
				else if (y > dirtHeight) blockID = data::Blocks.get("dirt").getID();
				else blockID = data::Blocks.get("stone").getID();
					
				data[x][y][z] = blockID;
			}
		}
	}
}
