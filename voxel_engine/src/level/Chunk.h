#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "structures/Attribute.h"
#include "data/Vertex.h"

namespace lvl {

	class World;
	struct ChunkID;

	using T = int;
	using VoxelData = std::vector<std::vector<std::vector<T>>>;


	struct ChunkID : glm::ivec3
	{
		ChunkID(int x, int y, int z) : glm::ivec3(x, y, z) {}

		bool operator()(const ChunkID& a, const ChunkID& b) const;
	};


	class Chunk
	{
	public:

		Chunk(World* world, ChunkID coords);
		//~Chunk();

		void populate();
		void generateMesh();

		/* @returns number of triangles to render */
		GLsizei prepareRender(ShaderPipeline* pipeline);

	private:

		ChunkID m_coords;
		VoxelData m_data;
		World* m_world;

		Attributes m_vertexData;
	};


}


template <>
struct std::hash<lvl::ChunkID> {
	size_t operator()(const glm::ivec3& k)const
	{
		return std::hash<int>()(k.x) ^ std::hash<int>()(k.y << 1) ^ std::hash<int>()(k.z << 2);
	}
};