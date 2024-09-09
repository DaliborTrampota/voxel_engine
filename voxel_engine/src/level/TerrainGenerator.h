#pragma once

#include <glm/glm.hpp>

#include "PerlinNoise.h"

class TerrainGenerator
{
public:
	TerrainGenerator() = default;


	virtual int getVoxel(glm::ivec3 pos) = 0;

};

class NoiseGenerator : public TerrainGenerator
{
public:
	NoiseGenerator() = default;
	NoiseGenerator(siv::PerlinNoise::seed_type seed) : noise(seed) {};

	int getVoxel(glm::ivec3 pos) override;

protected:
	siv::BasicPerlinNoise<float> noise;
};

