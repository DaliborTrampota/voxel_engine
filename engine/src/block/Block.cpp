#pragma once

#include "Block.h"

using namespace engine;

Block::Block(BlockID id, Layer layer, const Geometry* geo)
    : m_id(id),
      m_isSolid(true),
      m_isVoxel(false),
      m_layer(layer),
      m_geometry(geo) {}

engine::Block::Block(BlockID id, Layer layer, const Geometry* geo, RotationMode rotationMode)
    : Block(id, layer, geo) {
    m_rotationMode = rotationMode;
}

Block& engine::Block::air() {
    static Block airBlock = Block(0, Layers::Any, nullptr).isSolid(false);
    return airBlock;
}

Block& Block::rotationMode(RotationMode mode) {
    m_rotationMode = mode;
    return *this;
}

Block& Block::isSolid(bool solid) {
    m_isSolid = solid;
    return *this;
}

Block& Block::isVoxel(bool voxel) {
    m_isVoxel = voxel;
    return *this;
}

Block& Block::facingUp(bool state) {
    m_faceUp = state;
    return *this;
}

Block& Block::material(const BlockMaterial& mat) {
    m_material = mat;
    return *this;
}

const BlockMaterial& Block::material() const {
    return m_material;
}