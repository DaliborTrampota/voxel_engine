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

using namespace engine;

void RegisterBlocks() {
    RegistryManager::GeometryRegistryT& geometries = RegistryManager::Geometries();
    RegistryManager::BlockRegistryT& blocks = RegistryManager::Blocks();
    TextureManager& texMgr = TextureManager::Get();

    auto makeLog = [&geometries, &texMgr](
                       int id, int width, std::unordered_set<BlockID> connectedBlocks
                   ) -> VariantBlock {
        auto makeVariant = [&](Side side) -> VariantBlock::Variant {
            std::string name = std::format("oak_log_connector_{}", static_cast<int>(side));
            if (!geometries.has(name.c_str())) {
                Geometry geo = CreateRotatedGeometry(
                    geometries.get("oak_log_connector"),
                    {0, 1, 0},
                    getAngleToSide(side, -NORTH)  // The log geometry is pointing -NORTH
                );
                geometries.add(geo, name);
            }
            return VariantBlock::Variant(geometries.get(name.c_str()), {{side, connectedBlocks}});
        };
        auto log = VariantBlock(
                       id,
                       Layers::Opaque,
                       &geometries.get(std::format("log_{}", width).c_str()),
                       RotationMode::AxisXYZSnap,
                       4
        )
                       .allowMultiple(true)
                       .alwaysUseBaseGeometry(true)
                       .addVariant(makeVariant(Side::North))
                       .addVariant(makeVariant(Side::South))
                       .addVariant(makeVariant(Side::East))
                       .addVariant(makeVariant(Side::West));

        log.isSolid(true).isVoxel(true).material(
            BlockMaterial()
                .add(FaceTag::Side, texMgr.texture("log_oak"))
                .add(FaceTag::Top, texMgr.texture("log_oak_top"))
                .add(FaceTag::Bottom, texMgr.texture("log_oak_top"))
        );
        return log;
    };

    // clang-format off
    
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

    auto LOG_CONNECTOR = Block(14, Layers::Opaque, &geometries.get("oak_log_connector"))
        .isSolid(true)
        .isVoxel(true)
        .material(BlockMaterial()
            .add(FaceTag::All, texMgr.texture("log_oak"))
        );

    auto LOG_BRANCH = Block(15, Layers::Opaque, &geometries.get("oak_log_branch"))
        .isSolid(true)
        .isVoxel(true)
        .material(BlockMaterial()
            .add(FaceTag::All, texMgr.texture("log_oak"))
        );

        
    std::unordered_set<BlockID> logConnectedBlocks = {13, LOG_BRANCH.getID()};
    auto LOG_4 = makeLog(13, 4, logConnectedBlocks);
    auto LOG_6 = makeLog(12, 6, logConnectedBlocks);
    auto LOG_8 = makeLog(11, 8, logConnectedBlocks);
    auto LOG_10 = makeLog(10, 10, logConnectedBlocks);
    auto LOG_12 = makeLog(9, 12, logConnectedBlocks);
    auto LOG_14 = makeLog(8, 14, logConnectedBlocks);

    blocks.add(Block::air(), "air");
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
    blocks.add(LOG_CONNECTOR, "log_connector");
    blocks.add(LOG_BRANCH, "log_branch");
    // clang-format on
}

/*Blocks->add(Block(2, "grass_block", Geometry::CubeT("grass_top", "dirt")));
Blocks->add(Block(3, "stone", Geometry::CubeA("stone")));
Blocks->add(Block(3, "oak_log", Geometry::CubeC("oak_log_top", "oak_log")));
Blocks->add(Block(3, "oak_stairs", Geometry::Stair("oak_log_top", "oak_log")));
Blocks->add(Block(3, "table", table));*/
