#include "World.h"

using namespace lvl;

World::World(TerrainGenerator* gen) : generator(gen)
{

}

lvl::World::~World()
{
	delete generator;
}
