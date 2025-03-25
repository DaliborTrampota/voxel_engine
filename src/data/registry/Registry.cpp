#include "Registry.h"

#include "block/Block.h"
#include "block/builder/Geometry.h"
#include "data/TextureManager.h"

namespace data {
	//Global variable instead of singleton as per Cherno's advice :)
	//GeometryRegistry* Geometries = nullptr;
	Registry<Block> Blocks;
	extern TextureManager textureManager;



}

namespace registry {
	using namespace data;
	void Init()
	{
		//InitGeometries();
		InitBlocks();

	}

	void Clean()
	{
		//delete Blocks;
		//delete Geometries;
	}

	//void InitGeometries()
	//{
	//	Geometries = new GeometryRegistry();

	//	Geometries->add("cube", new builder::CubeGeometry(1));
	//	//data::Geometries->add("culled", new builder::CulledGeometry());
	//}

	void InitBlocks()
	{
		builder::Geometry pyramid;
		pyramid.m_faces.push_back(builder::Face::SquareFace(glm::vec3(0, 0, 0), glm::vec3(1, 0, 1), glm::vec3(0, -1, 0), glm::vec2(0, 0), glm::vec2(1, 1), data::textureManager.get("cobblestone")));
		pyramid.m_faces.push_back(builder::Face::TriangleFace(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0.5f, 1, 0.5f), glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(0.5f, 1), data::textureManager.get("cobblestone")));
		pyramid.m_faces.push_back(builder::Face::TriangleFace(glm::vec3(0, 0, 1), glm::vec3(1, 0, 1), glm::vec3(0.5f, 1, 0.5f), glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(0.5f, 1), data::textureManager.get("cobblestone")));
		pyramid.m_faces.push_back(builder::Face::TriangleFace(glm::vec3(1, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0.5f, 1, 0.5f), glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(0.5f, 1), data::textureManager.get("cobblestone")));
		pyramid.m_faces.push_back(builder::Face::TriangleFace(glm::vec3(1, 0, 1), glm::vec3(1, 0, 0), glm::vec3(0.5f, 1, 0.5f), glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(0.5f, 1), data::textureManager.get("cobblestone")));

		Blocks.add(Block(0, "air", builder::Geometry()));
		Blocks.add(Block(1, "dirt", builder::Geometry::Cube("dirt")).isSolid(true));
		Blocks.add(Block(2, "grass", builder::Geometry::Cube("grass_block_top", "dirt", "grass_block_side")).isSolid(true));
		Blocks.add(Block(3, "stone", builder::Geometry::Cube("stone")).isSolid(true));

		Blocks.add(Block(4, "cylinder", builder::Geometry::Cylinder("cobblestone", "stone", "sandstone")).isSolid(true).isVoxel(true));
		Blocks.add(Block(5, "pyramid", pyramid).isSolid(true).isVoxel(true));



		/*Blocks->add(Block(2, "grass_block", builder::Geometry::CubeT("grass_top", "dirt")));
		Blocks->add(Block(3, "stone", builder::Geometry::CubeA("stone")));
		Blocks->add(Block(3, "oak_log", builder::Geometry::CubeC("oak_log_top", "oak_log")));
		Blocks->add(Block(3, "oak_stairs", builder::Geometry::Stair("oak_log_top", "oak_log")));
		Blocks->add(Block(3, "table", table))*/;
	}
}