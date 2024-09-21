#pragma once

#include <glm/glm.hpp>

#include "PerlinNoise.h"

class TerrainGenerator
{
public:
	TerrainGenerator() = default;

	using T = unsigned int;

	virtual int getVoxel(glm::ivec3 pos) = 0;
	virtual int getHeight(glm::ivec2 pos) = 0;

	virtual void populate(std::vector<std::vector<std::vector<T>>> &data, glm::ivec3 posOffset) = 0;
	
};

class NoiseGenerator : public TerrainGenerator
{
public:
	NoiseGenerator() = default;
	NoiseGenerator(siv::PerlinNoise::seed_type seed) : noise(seed) {};

	int getVoxel(glm::ivec3 pos) override;
	int getHeight(glm::ivec2 pos) override;

	void populate(std::vector<std::vector<std::vector<T>>> &data, glm::ivec3 posOffset) override;

protected:
	siv::BasicPerlinNoise<float> noise;

	float m_scale = 0.1f;
	float m_minHeight = 5.0f;

};

