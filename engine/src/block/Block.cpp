#pragma once

#include "Block.h"

using namespace engine;

Block::Block(BlockID id, const Geometry* geo)
    : m_id(id),
      m_isOpaque(false),
      m_isSolid(false),
      m_isVoxel(false),
      m_geometry(geo) {}

engine::Block::Block(BlockID id, const Geometry* geo, const BlockMaterial& mat) : Block(id, geo) {
    m_material = mat;
}

Block& engine::Block::air() {
    static Block airBlock = Block(0, nullptr).isSolid(false).isOpaque(false);
    return airBlock;
}
