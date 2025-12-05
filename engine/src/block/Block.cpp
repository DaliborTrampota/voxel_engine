#pragma once

#include "Block.h"

using namespace engine;

Block::Block(BlockID id, Layer layer, const Geometry* geo)
    : m_id(id),
      m_isSolid(true),
      m_isVoxel(false),
      m_layer(layer),
      m_geometry(geo) {}

engine::Block::Block(BlockID id, Layer layer, const Geometry* geo, const BlockMaterial& mat)
    : Block(id, layer, geo) {
    m_material = mat;
}

Block& engine::Block::air() {
    static Block airBlock = Block(0, Layers::Any, nullptr).isSolid(false);
    return airBlock;
}
