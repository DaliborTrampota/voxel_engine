#include "Blocks.h"

#include <data/Registry.h>
#include <data/RegistryManager.h>
#include <data/TextureManager.h>

#include <block/Block.h>
#include <block/BlockMaterial.h>
#include <block/Geometry.h>

using namespace engine;

void RegisterBlocks() {
    // clang-format off
    Registry<Geometry>& geometries = RegistryManager::Geometries();

    geometries.add(Geometry::Cube(), "cube");
    geometries.add(Geometry::Cylinder(), "cylinder");
    geometries.add(Pyramid(), "pyramid");



    Registry<Block>& blocks = RegistryManager::Blocks();
    TextureManager& texMgr = TextureManager::Get();
    
    auto DIRT = Block(1, Layers::Opaque, &geometries.get("cube"))
        .isSolid(true)
        .material(BlockMaterial()
            .add(FaceTag::All, texMgr.texture("dirt"))
        );
    
    auto GRASS = Block(2, Layers::Opaque, &geometries.get("cube"))
        .isSolid(true)
        .material(BlockMaterial()
            .add(FaceTag::Top, texMgr.texture("grass_block_top"))
            .add(FaceTag::Bottom, texMgr.texture("dirt"))
            .add(FaceTag::Side, texMgr.texture("grass_block_side"))
        );
    
    auto STONE = Block(3, Layers::Opaque, &geometries.get("cube"))
        .isSolid(true)
        .material(BlockMaterial()
            .add(FaceTag::All, texMgr.texture("stone"))
        );

    auto SAND = Block(4, Layers::Opaque, &geometries.get("cube"))
        .isSolid(true)
        .material(BlockMaterial()
            .add(FaceTag::All, texMgr.texture("sand"))
        );
    
    auto CYLINDER = Block(5, Layers::Opaque, &geometries.get("cylinder"))
        .isSolid(true)
        .isVoxel(true)
        .material(BlockMaterial()
            .add(FaceTag::All, texMgr.texture("stone"))
        );
    auto PYRAMID = Block(6, Layers::Opaque, &geometries.get("pyramid"))
        .isSolid(true)
        .isVoxel(true)
        .material(BlockMaterial()
            .add(FaceTag::All, texMgr.texture("stone"))
        );

    auto GLASS = Block(7, Layers::Transparent, &geometries.get("cube"))
        .isSolid(true)
        .material(BlockMaterial()
            .add(FaceTag::All, texMgr.texture("glass"))
        );

    blocks.add(Block::air(), "air");
    blocks.add(DIRT, "dirt");
    blocks.add(GRASS, "grass");
    blocks.add(STONE, "stone");
    blocks.add(SAND, "sand");
    blocks.add(CYLINDER, "cylinder");
    blocks.add(PYRAMID, "pyramid");
    blocks.add(GLASS, "glass");
    // clang-format on
}

/*Blocks->add(Block(2, "grass_block", Geometry::CubeT("grass_top", "dirt")));
Blocks->add(Block(3, "stone", Geometry::CubeA("stone")));
Blocks->add(Block(3, "oak_log", Geometry::CubeC("oak_log_top", "oak_log")));
Blocks->add(Block(3, "oak_stairs", Geometry::Stair("oak_log_top", "oak_log")));
Blocks->add(Block(3, "table", table));*/

Geometry Pyramid() {
    std::vector<Face> faces;
    faces.push_back(
        Face::SquareFace(
            FaceTag::Bottom,
            glm::vec3(0, 0, 0),
            glm::vec3(1, 0, 1),
            glm::vec3(0, -1, 0),
            glm::vec2(0, 0),
            glm::vec2(1, 1)
        )
    );
    faces.push_back(
        Face::TriangleFace(
            FaceTag::Side,
            glm::vec3(0, 0, 0),
            glm::vec3(0, 0, 1),
            glm::vec3(0.5f, 1, 0.5f),
            glm::vec2(0, 0),
            glm::vec2(1, 0),
            glm::vec2(0.5f, 1)
        )
    );
    faces.push_back(
        Face::TriangleFace(
            FaceTag::Side,
            glm::vec3(0, 0, 1),
            glm::vec3(1, 0, 1),
            glm::vec3(0.5f, 1, 0.5f),
            glm::vec2(0, 0),
            glm::vec2(1, 0),
            glm::vec2(0.5f, 1)
        )
    );
    faces.push_back(
        Face::TriangleFace(
            FaceTag::Side,
            glm::vec3(1, 0, 0),
            glm::vec3(0, 0, 0),
            glm::vec3(0.5f, 1, 0.5f),
            glm::vec2(0, 0),
            glm::vec2(1, 0),
            glm::vec2(0.5f, 1)
        )
    );
    faces.push_back(
        Face::TriangleFace(
            FaceTag::Side,
            glm::vec3(1, 0, 1),
            glm::vec3(1, 0, 0),
            glm::vec3(0.5f, 1, 0.5f),
            glm::vec2(0, 0),
            glm::vec2(1, 0),
            glm::vec2(0.5f, 1)
        )
    );
    return Geometry(faces);
}