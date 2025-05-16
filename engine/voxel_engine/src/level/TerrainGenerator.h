#pragma once

#include <glm/glm.hpp>

#include "data/Registry.h"
#include "block/Block.h"
#include "tools/PerlinNoise.h"

namespace engine {


	class TerrainGenerator
	{
	public:
		TerrainGenerator() = default;

		using T = unsigned int;

		virtual int getVoxel(glm::ivec3 pos) const = 0;
		virtual int getHeight(glm::ivec2 pos) const = 0;

		virtual void populate(std::vector<std::vector<std::vector<T>>>& data, glm::ivec3 posOffset) = 0;

	};

	class NoiseGenerator : public TerrainGenerator
	{
	public:
		NoiseGenerator();
		NoiseGenerator(siv::PerlinNoise::seed_type seed);

		int getVoxel(glm::ivec3 pos) const override;
		int getHeight(glm::ivec2 pos) const override;
		int getVoxel(glm::ivec3 pos, int y) const;

		void populate(std::vector<std::vector<std::vector<T>>>& data, glm::ivec3 posOffset) override;

	protected:
		siv::BasicPerlinNoise<float> noise;

		const Registry<Block>*  m_blockRegistry;

		float m_scale = 0.1f;
		float m_minHeight = 5.0f;

	};
}