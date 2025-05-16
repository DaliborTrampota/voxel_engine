#pragma once

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

#include <gl/Attributes.h>
#include <block/BlockData.h>
#include <block/Block.h>

#include <thread>

namespace gl {
	class ShaderPipeline;
}

namespace engine {

	class World;

	using T = unsigned int;
	using VoxelData = std::vector<std::vector<std::vector<T>>>;


	struct ChunkID : glm::ivec3
	{
		ChunkID(int x, int y, int z) : glm::ivec3(x, y, z) {}

		bool operator()(const ChunkID& a, const ChunkID& b) const;
		bool operator==(const ChunkID& other) const;
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

		Block getBlock(glm::ivec3 pos) const;
	private:

		World* m_world;
		ChunkID m_coords;

		VoxelData m_data;
		//std::unordered_map<ChunkID, BlockData> m_metadata;

		gl::Attributes<Vertex> m_vertexData;

		std::thread m_genThread;
		bool m_generated = false;
        
        friend class Engine;
	};


}

namespace std {
	template <>
	struct hash<engine::ChunkID>
	{
		size_t operator()(const engine::ChunkID& k)const
		{
			return hash<int>()(k.x) ^ hash<int>()(k.y << 1) ^ hash<int>()(k.z << 2);
		}
	};

}
