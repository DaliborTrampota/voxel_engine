#pragma once

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

#include "structures/gl/Attributes.h"
#include "block/BlockData.h"
#include "block/Block.h"

#include <thread>

class ShaderPieline;



namespace lvl {

	class World;

	using T = unsigned int;
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
		Chunk(Chunk&) = delete;
		Chunk(Chunk&&) = delete;
		~Chunk();

		ChunkID getID() const { return m_coords; }
		void populate();
		void generateMesh();
		void generate();
		bool generated() const { return m_generated; }

		data::Block getBlock(glm::ivec3 pos) const;

		/* @returns number of triangles to render */
		GLsizei prepareRender(ShaderPipeline* pipeline);

	private:

		World* m_world;
		ChunkID m_coords;

		VoxelData m_data;
		//std::unordered_map<lvl::ChunkID, data::BlockData> m_metadata;

		Attributes m_vertexData;

		std::thread m_genThread;
		bool m_generated = false;
	};


}


template <>
struct std::hash<lvl::ChunkID>
{
	size_t operator()(const glm::ivec3& k)const
	{
		return std::hash<int>()(k.x) ^ std::hash<int>()(k.y << 1) ^ std::hash<int>()(k.z << 2);
	}
};