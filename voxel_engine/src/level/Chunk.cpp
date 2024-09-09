#include "Chunk.h"

using namespace lvl;

void Chunk::populate()
{
	for (int x = 0; x < 16; x++)
	{
		data.push_back(std::vector<std::vector<T>>());
		for (int y = 0; y < 16; y++)
		{
			data[x].push_back(std::vector<T>());
			for (int z = 0; z < 16; z++)
			{
				data[x][y].push_back(0);
			}
		}
	}
}
