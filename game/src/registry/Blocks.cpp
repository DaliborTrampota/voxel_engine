#include "Blocks.h"

#include <format>
#include <glm/gtc/matrix_transform.hpp>

#include <data/Registry.h>
#include <data/RegistryManager.h>
#include <data/TextureManager.h>

#include <block/Block.h>
#include <block/BlockMaterial.h>
#include <block/Geometry.h>
#include <block/VariantBlock.h>

#include <utility/CoordUtils.h>

#include "Geometries.h"
#include "MyRegistryManager.h"

using namespace engine;

void RegisterBlocks() {
    RegistryManager::GeometryRegistryT& geometries = MyRegistryManager::Geometries();
    RegistryManager::BlockRegistryT& blocks = MyRegistryManager::Blocks();
    TextureManager& texMgr = TextureManager::Get();

    auto makeLog = [&geometries, &texMgr](
                       int id, int width, std::vector<BlockID> connectedBlocks
                   ) -> VariantBlock {
        auto makeVariant = [&](Side side) -> VariantBlock::Variant {
            std::string name = std::format("oak_log_connector_{}", static_cast<int>(side));
            if (!geometries.has(name.c_str())) {
                glm::vec3 axis;
                float angle = getAngleToSide(
                    side, NORTH, axis
                );  // The log connector geometry is pointing NORTH
                Geometry geo =
                    CreateRotatedGeometry(geometries.get("oak_log_connector"), axis, angle);
                geometries.add(geo, name);
            }
            return VariantBlock::Variant(geometries.get(name.c_str()), {{side, connectedBlocks}});
        };
        auto log = VariantBlock(
                       id,
                       Layers::Opaque,
                       &geometries.get(std::format("log_{}", width).c_str()),
                       RotationMode::AxisAlign,
                       4
        )
                       .allowMultiple(true)
                       .alwaysUseBaseGeometry(true)
                       .addVariant(makeVariant(Side::North))
                       .addVariant(makeVariant(Side::South))
                       .addVariant(makeVariant(Side::East))
                       .addVariant(makeVariant(Side::West));

        log.isSolid(true).isVoxel(true).facingUp(true).material(
            BlockMaterial()
                .add(FaceTag::Side, texMgr.texture("log_oak"))
                .add(FaceTag::Top, texMgr.texture("log_oak_top"))
                .add(FaceTag::Bottom, texMgr.texture("log_oak_top"))
        );
        return log;
    };

    // clang-format off
    auto DIRT = Block(8, Layers::Opaque, &geometries.get("cube"))
        .isSolid(true)
        .material(BlockMaterial()
            .add(FaceTag::All, texMgr.texture("dirt"))
        );
    
    auto GRASS = Block(9, Layers::Opaque, &geometries.get("cube"))
        .isSolid(true)
        .material(BlockMaterial()
            .add(FaceTag::Top, texMgr.texture("grass_block_top"))
            .add(FaceTag::Bottom, texMgr.texture("dirt"))
            .add(FaceTag::Side, texMgr.texture("grass_block_side"))
        );
    
    auto STONE = Block(10, Layers::Opaque, &geometries.get("cube"))
        .isSolid(true)
        .material(BlockMaterial()
            .add(FaceTag::All, texMgr.texture("stone"))
        );

    auto SAND = Block(11, Layers::Opaque, &geometries.get("cube"))
        .isSolid(true)
        .material(BlockMaterial()
            .add(FaceTag::All, texMgr.texture("sand"))
        );
    
    auto CYLINDER = Block(12, Layers::Opaque, &geometries.get("cylinder"))
        .isSolid(true)
        .isVoxel(true)
        .material(BlockMaterial()
            .add(FaceTag::All, texMgr.texture("stone"))
        );
    auto PYRAMID = Block(13, Layers::Opaque, &geometries.get("pyramid"))
        .isSolid(true)
        .isVoxel(true)
        .material(BlockMaterial()
            .add(FaceTag::All, texMgr.texture("stone"))
        );

    auto GLASS = Block(14, Layers::Transparent, &geometries.get("cube"))
        .isSolid(true)
        .material(BlockMaterial()
            .add(FaceTag::All, texMgr.texture("glass"))
        );

    // auto LOG_CONNECTOR = Block(21, Layers::Opaque, &geometries.get("oak_log_connector"))
    //     .isSolid(true)
    //     .isVoxel(true)
    //     .material(BlockMaterial()
    //         .add(FaceTag::All, texMgr.texture("log_oak"))
    //     );


    auto LOG_BRANCH = Block(21, Layers::Opaque, &geometries.get("oak_log_branch"), RotationMode::AxisYSnap)
        .isSolid(true)
        .isVoxel(true)
        .material(BlockMaterial()
            .add(FaceTag::All, texMgr.texture("log_oak"))
        );

        
    auto LEAVES = Block(22, Layers::Transparent, &geometries.get("cube"))
    .isSolid(true)
    .material(BlockMaterial()
        .add(FaceTag::All, texMgr.texture("oak_leaves"))
    );

    // clang-format on

    std::vector<BlockID> logConnectedBlocks = {20 /* LOG_4 */, LOG_BRANCH.getID()};
    auto LOG_6 = makeLog(19, 6, logConnectedBlocks);
    auto LOG_8 = makeLog(18, 8, logConnectedBlocks);
    auto LOG_10 = makeLog(17, 10, logConnectedBlocks);
    auto LOG_12 = makeLog(16, 12, logConnectedBlocks);
    auto LOG_14 = makeLog(15, 14, logConnectedBlocks);

    auto LOG_4 = makeLog(
        20, 4, {20, LOG_6.getID(), LOG_8.getID(), LOG_10.getID(), LOG_12.getID(), LOG_14.getID()}
    );

    auto VEGETATION_CROSS =
        Block(25, Layers::Opaque, &geometries.get("vegetation_cross"))
            .isSolid(true)
            .isVoxel(true)
            .material(BlockMaterial().add(FaceTag::All, texMgr.texture("calcite")));
    auto VEGETATION_SQUARE =
        Block(26, Layers::Opaque, &geometries.get("vegetation_square"))
            .isSolid(true)
            .isVoxel(true)
            .material(BlockMaterial().add(FaceTag::All, texMgr.texture("calcite")));
    auto VEGETATION_HASH =
        Block(27, Layers::Opaque, &geometries.get("vegetation_hash"))
            .isSolid(true)
            .isVoxel(true)
            .material(BlockMaterial().add(FaceTag::All, texMgr.texture("calcite")));


    blocks.add(DIRT, "dirt");
    blocks.add(GRASS, "grass");
    blocks.add(STONE, "stone");
    blocks.add(SAND, "sand");
    blocks.add(CYLINDER, "cylinder");
    blocks.add(PYRAMID, "pyramid");
    blocks.add(GLASS, "glass");
    blocks.add(LOG_4, "log_4");
    blocks.add(LOG_6, "log_6");
    blocks.add(LOG_8, "log_8");
    blocks.add(LOG_10, "log_10");
    blocks.add(LOG_12, "log_12");
    blocks.add(LOG_14, "log_14");
    // blocks.add(LOG_CONNECTOR, "log_connector");
    blocks.add(LOG_BRANCH, "log_branch");
    blocks.add(LEAVES, "leaves");
    blocks.add(VEGETATION_CROSS, "vegetation_cross");
    blocks.add(VEGETATION_SQUARE, "vegetation_square");
    blocks.add(VEGETATION_HASH, "vegetation_hash");
}

/*Blocks->add(Block(2, "grass_block", Geometry::CubeT("grass_top", "dirt")));
Blocks->add(Block(3, "stone", Geometry::CubeA("stone")));
Blocks->add(Block(3, "oak_log", Geometry::CubeC("oak_log_top", "oak_log")));
Blocks->add(Block(3, "oak_stairs", Geometry::Stair("oak_log_top", "oak_log")));
Blocks->add(Block(3, "table", table));*/


void RegisterMultiBlocks() {
    RegistryManager::BlockRegistryT& blocks = MyRegistryManager::Blocks();
    MyRegistryManager::MultiBlockCombinationRegistryT& multiBlockCombinations =
        MyRegistryManager::MultiBlockCombinations();

    multiBlockCombinations.add(
        MultiBlockCombination(
            {{blocks.get("log_4")->getID(), false}, {blocks.get("leaves")->getID(), false}}
        ),
        "leaves_branch"
    );
}