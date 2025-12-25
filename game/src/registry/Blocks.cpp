#include "Blocks.h"

#include <format>
#include <glm/gtc/matrix_transform.hpp>

#include <Globals.h>
#include <data/Registry.h>
#include <data/RegistryManager.h>
#include <data/TextureManager.h>

#include <block/Block.h>
#include <block/BlockMaterial.h>
#include <block/Geometry.h>
#include <block/VariantBlock.h>

#include <utility/Rotation.h>

#include "Geometries.h"
#include "MyRegistryManager.h"

using namespace engine;

// namespace {
//     VariantBlock makeLog(int id, int width, std::vector<BlockID> connectedBlocks) {
//         RegistryManager::GeometryRegistryT& geometries = MyRegistryManager::Geometries();
//         TextureManager& texMgr = TextureManager::Get();

//         auto makeVariant = [&](Side side) -> VariantBlock::Variant {
//             std::string name = std::format("log_connector_{}", static_cast<int>(side));
//             if (!geometries.has(name.c_str())) {
//                 glm::vec3 axis;
//                 // The log connector geometry is pointing NORTH
//                 float angle = getAngleToSide(side, NORTH, axis);
//                 Geometry geo =
//                     CreateRotatedGeometry(geometries.get("log_connector"), axis, angle);
//                 geometries.add(geo, name);
//             }
//             return VariantBlock::Variant(geometries.get(name.c_str()), {{side, connectedBlocks}});
//         };
//         auto log = VariantBlock(
//                        id,
//                        Layers::Opaque,
//                        &geometries.get(std::format("log_{}", width).c_str()),
//                        RotationMode::AxisAlign,
//                        4
//         )
//                        .allowMultiple(true)
//                        .alwaysUseBaseGeometry(true)
//                        .addVariant(makeVariant(Side::North))
//                        .addVariant(makeVariant(Side::South))
//                        .addVariant(makeVariant(Side::East))
//                        .addVariant(makeVariant(Side::West));

//         log.isSolid(true).isVoxel(true).facingUp(true).material(
//             BlockMaterial()
//                 .add(FaceTag::Side, texMgr.texture("log_oak"))
//                 .add(FaceTag::Top, texMgr.texture("log_top"))
//                 .add(FaceTag::Bottom, texMgr.texture("log_top"))
//         );
//         return log;
//     };
// }  // namespace

void RegisterBlocks() {
    RegistryManager::GeometryRegistryT& geometries = MyRegistryManager::Geometries();
    RegistryManager::BlockRegistryT& blocks = MyRegistryManager::Blocks();
    TextureManager& texMgr = TextureManager::Get();


    auto ASH = Block(8, Layers::Opaque, &geometries.get("cube"))
                   .isSolid(true)
                   .material(BlockMaterial().add(FaceTag::All, texMgr.texture("ash")));

    auto COMPRESSED_ASH =
        Block(9, Layers::Opaque, &geometries.get("cube"))
            .isSolid(true)
            .material(BlockMaterial().add(FaceTag::All, texMgr.texture("compressed_ash")));

    auto SILICATE_SAND =
        Block(10, Layers::Opaque, &geometries.get("cube"))
            .isSolid(true)
            .material(BlockMaterial().add(FaceTag::All, texMgr.texture("silicate_sand")));

    auto BASALT = Block(11, Layers::Opaque, &geometries.get("cube"))
                      .isSolid(true)
                      .material(BlockMaterial().add(FaceTag::All, texMgr.texture("basalt")));

    auto FRACTURED_BASALT =
        Block(12, Layers::Opaque, &geometries.get("cube"))
            .isSolid(true)
            .material(BlockMaterial().add(FaceTag::All, texMgr.texture("fractured_basalt")));

    auto SLATE = Block(13, Layers::Opaque, &geometries.get("cube"))
                     .isSolid(true)
                     .material(BlockMaterial().add(FaceTag::All, texMgr.texture("slate")));

    auto CRYSTAL_CRUST =
        Block(14, Layers::Opaque, &geometries.get("cube"))
            .isSolid(true)
            .material(BlockMaterial().add(FaceTag::All, texMgr.texture("crystal_crust")));

    auto MINERAL = Block(15, Layers::Opaque, &geometries.get("cube"))
                       .isSolid(true)
                       .material(BlockMaterial().add(FaceTag::All, texMgr.texture("mineral")));

    auto GAS_ICE = Block(16, Layers::Opaque, &geometries.get("cube"))
                       .isSolid(true)
                       .material(BlockMaterial().add(FaceTag::All, texMgr.texture("gas_ice")));

    auto CO2_GAS_ICE =
        Block(17, Layers::Opaque, &geometries.get("cube"))
            .isSolid(true)
            .material(BlockMaterial().add(FaceTag::All, texMgr.texture("co2_gas_ice")));

    auto O2_GAS_ICE =
        Block(18, Layers::Opaque, &geometries.get("cube"))
            .isSolid(true)
            .material(BlockMaterial().add(FaceTag::All, texMgr.texture("o2_gas_ice")));

    auto DEEP_SLATE =
        Block(19, Layers::Opaque, &geometries.get("cube"))
            .isSolid(true)
            .material(BlockMaterial().add(FaceTag::All, texMgr.texture("deep_slate")));

    auto IRON_BASALT =
        Block(20, Layers::Opaque, &geometries.get("cube"))
            .isSolid(true)
            .material(BlockMaterial().add(FaceTag::All, texMgr.texture("iron_basalt")));

    auto MICROBIAL_MATTER =
        Block(21, Layers::Opaque, &geometries.get("cube"))
            .isSolid(true)
            .material(BlockMaterial().add(FaceTag::All, texMgr.texture("microbial_crust")));

    auto ALGAL_MATTER = Block(22, Layers::Opaque, &geometries.get("cube"))
                            .isSolid(true)
                            .material(BlockMaterial().add(FaceTag::All, texMgr.texture("algal")));


    // clang-format off
    auto makeLog = [&geometries, &texMgr](int id, int width, std::vector<BlockID> connectedBlocks) -> VariantBlock {
        auto makeVariant = [&](Side side) -> VariantBlock::Variant {
            std::string name = std::format("log_connector_{}", static_cast<int>(side));
            if (!geometries.has(name.c_str())) {
                glm::vec3 axis;
                // The log connector geometry is pointing NORTH
                float angle = getAngleToSide(side, NORTH, axis);  
                Geometry geo = CreateRotatedGeometry(geometries.get("log_connector"), axis, angle);
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
                .add(FaceTag::Side, texMgr.texture("log"))
                .add(FaceTag::Top, texMgr.texture("log_top"))
                .add(FaceTag::Bottom, texMgr.texture("log_top"))
        );
        return log;
    };

    auto DIRT = Block(33, Layers::Opaque, &geometries.get("cube"))
        .isSolid(true)
        .material(BlockMaterial()
            .add(FaceTag::All, texMgr.texture("dirt"))
        );
    
    auto GRASS = Block(34, Layers::Opaque, &geometries.get("cube"))
        .isSolid(true)
        .material(BlockMaterial()
            .add(FaceTag::Top, texMgr.texture("grass_block_top"))
            .add(FaceTag::Bottom, texMgr.texture("dirt"))
            .add(FaceTag::Side, texMgr.texture("grass_block_side"))
        );

    auto GLASS = Block(35, Layers::Transparent, &geometries.get("cube"))
        .isSolid(true)
        .material(BlockMaterial()
            .add(FaceTag::All, texMgr.texture("glass"))
        );

    auto STONE = Block(36, Layers::Opaque, &geometries.get("cube"))
        .isSolid(true)
        .material(BlockMaterial().add(FaceTag::All, texMgr.texture("stone")));

    // auto TUFF = Block(16, Layers::Opaque, &geometries.get("cube"))
    //     .isSolid(true)
    //     .material(BlockMaterial()
    //         .add(FaceTag::All, texMgr.texture("tuff"))
    //     );

    // auto CLAY = Block(17, Layers::Opaque, &geometries.get("cube"))
    //     .isSolid(true)
    //     .material(BlockMaterial()
    //         .add(FaceTag::All, texMgr.texture("hardened_clay"))
    //     );

    // auto CALCITE = Block(18, Layers::Opaque, &geometries.get("cube"))
    //     .isSolid(true)
    //     .material(BlockMaterial()
    //         .add(FaceTag::All, texMgr.texture("calcite"))
    //     );



    auto LOG_BRANCH = Block(64, Layers::Opaque, &geometries.get("log_branch"), RotationMode::AxisYSnap)
        .isSolid(true)
        .isVoxel(true)
        .material(BlockMaterial()
            .add(FaceTag::All, texMgr.texture("log"))
        );

        
    auto LEAVES = Block(65, Layers::Transparent, &geometries.get("cube"))
        .isSolid(true)
        .material(BlockMaterial()
            .add(FaceTag::All, texMgr.texture("leaves"))
        );

    // clang-format on
    std::vector<BlockID> logConnectedBlocks = {71 /* LOG_4 */, LOG_BRANCH.getID()};

    auto LOG_6 = makeLog(66, 6, logConnectedBlocks);
    auto LOG_8 = makeLog(67, 8, logConnectedBlocks);
    auto LOG_10 = makeLog(68, 10, logConnectedBlocks);
    auto LOG_12 = makeLog(69, 12, logConnectedBlocks);
    auto LOG_14 = makeLog(70, 14, logConnectedBlocks);

    auto LOG_4 = makeLog(
        71, 4, {70, LOG_6.getID(), LOG_8.getID(), LOG_10.getID(), LOG_12.getID(), LOG_14.getID()}
    );

    // auto GRASS_DECO =
    //     Block(23, Layers::Opaque, &geometries.get("grass.vox"), RotationMode::AxisY)
    //         .isSolid(false)
    //         .isVoxel(true)
    //         .material(BlockMaterial().add(FaceTag::All, texMgr.texture("grass_block_top")));


    // auto VEGETATION_CROSS =
    //     Block(25, Layers::Opaque, &geometries.get("vegetation_cross"))
    //         .isSolid(true)
    //         .isVoxel(true)
    //         .material(BlockMaterial().add(FaceTag::All, texMgr.texture("calcite")));
    // auto VEGETATION_SQUARE =
    //     Block(26, Layers::Opaque, &geometries.get("vegetation_square"))
    //         .isSolid(true)
    //         .isVoxel(true)
    //         .material(BlockMaterial().add(FaceTag::All, texMgr.texture("calcite")));
    // auto VEGETATION_HASH =
    //     Block(27, Layers::Opaque, &geometries.get("vegetation_hash"))
    //         .isSolid(false)
    //         .isVoxel(true)
    //         .material(BlockMaterial().add(FaceTag::All, texMgr.texture("calcite")));


    // Biomes
    blocks.add(ASH, "ash");
    blocks.add(SILICATE_SAND, "silicate_sand");
    blocks.add(BASALT, "basalt");
    blocks.add(SLATE, "slate");
    blocks.add(CRYSTAL_CRUST, "crystal_crust");
    blocks.add(MINERAL, "mineral");
    blocks.add(GAS_ICE, "gas_ice");
    blocks.add(CO2_GAS_ICE, "co2_gas_ice");
    blocks.add(O2_GAS_ICE, "o2_gas_ice");
    blocks.add(IRON_BASALT, "iron_basalt");
    blocks.add(MICROBIAL_MATTER, "microbial_matter");
    blocks.add(ALGAL_MATTER, "algal_slime");

    // Temp/Stolen textures
    blocks.add(DEEP_SLATE, "deep_slate");
    blocks.add(STONE, "stone");
    blocks.add(GLASS, "glass");
    blocks.add(FRACTURED_BASALT, "fractured_basalt");
    blocks.add(COMPRESSED_ASH, "compressed_ash");

    // Terraformed
    blocks.add(DIRT, "dirt");
    blocks.add(GRASS, "grass");

    // Vegetation
    blocks.add(LOG_4, "log_4");
    blocks.add(LOG_6, "log_6");
    blocks.add(LOG_8, "log_8");
    blocks.add(LOG_10, "log_10");
    blocks.add(LOG_12, "log_12");
    blocks.add(LOG_14, "log_14");
    blocks.add(LOG_BRANCH, "log_branch");
    blocks.add(LEAVES, "leaves");
}


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