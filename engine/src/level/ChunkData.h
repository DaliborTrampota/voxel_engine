#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "../Globals.h"

namespace engine {

    /**
     * ChunkData stores voxel information in a packed format:
     * - Lower 8 bits: Layer ID (0-255)
     * - Upper 24 bits: Block ID (0-16,777,215)
     * 
     * This allows efficient storage and quick layer-based rendering.
     */
    struct ChunkData {
        static constexpr uint32_t LAYER_MASK = 0xFF;        // Lower 8 bits
        static constexpr uint32_t BLOCK_MASK = 0xFFFFFF00;  // Upper 24 bits
        static constexpr int LAYER_SHIFT = 0;
        static constexpr int BLOCK_SHIFT = 8;

        bool populated = false;
        glm::ivec3 dims;
        std::vector<uint32_t> data;  // Packed: [BlockID:24][Layer:8]

        ChunkData() = default;
        ChunkData(glm::ivec3 dims) : dims(dims), data(dims.x * dims.y * dims.z, 0) {}

        // 3D to 1D index mapping (Z-Y-X order for cache locality)
        inline size_t index(int x, int y, int z) const { return x + dims.x * (y + dims.y * z); }

        inline size_t index(const glm::ivec3& pos) const { return index(pos.x, pos.y, pos.z); }

        // Raw data access (packed format)
        inline uint32_t& operator()(int x, int y, int z) { return data[index(x, y, z)]; }

        inline const uint32_t& operator()(int x, int y, int z) const {
            return data[index(x, y, z)];
        }

        inline uint32_t& operator()(const glm::ivec3& pos) { return data[index(pos)]; }

        inline const uint32_t& operator()(const glm::ivec3& pos) const { return data[index(pos)]; }

        // Block ID access (upper 24 bits)
        inline BlockID getBlock(int x, int y, int z) const {
            return (data[index(x, y, z)] & BLOCK_MASK) >> BLOCK_SHIFT;
        }

        inline BlockID getBlock(const glm::ivec3& pos) const {
            return getBlock(pos.x, pos.y, pos.z);
        }

        inline BlockID getBlockAndLayer(int x, int y, int z, Layer& layer) const {
            uint32_t blockData = data[index(x, y, z)];
            layer = blockData & LAYER_MASK;
            return (blockData & BLOCK_MASK) >> BLOCK_SHIFT;
        }

        inline BlockID getBlockAndLayer(const glm::ivec3& pos, Layer& layer) const {
            return getBlockAndLayer(pos.x, pos.y, pos.z, layer);
        }

        inline BlockID getBlockFromLayer(int x, int y, int z, Layer layer) const {
            if (layer == Layers::Any) {
                return getBlock(x, y, z);
            }
            uint32_t blockData = data[index(x, y, z)];
            if ((blockData & LAYER_MASK) == layer) {
                return (blockData & BLOCK_MASK) >> BLOCK_SHIFT;
            }
            return 0;
        }

        // Layer access (lower 8 bits)
        inline Layer getLayer(int x, int y, int z) const {
            return data[index(x, y, z)] & LAYER_MASK;
        }

        inline Layer getLayer(const glm::ivec3& pos) const { return getLayer(pos.x, pos.y, pos.z); }

        // Set block with layer
        inline void setBlock(int x, int y, int z, BlockID block, Layer layer) {
            data[index(x, y, z)] = (block << BLOCK_SHIFT) | layer;
        }

        inline void setBlock(const glm::ivec3& pos, BlockID block, Layer layer) {
            setBlock(pos.x, pos.y, pos.z, block, layer);
        }

        // Convenience: set block only (preserves layer)
        inline void setBlockOnly(int x, int y, int z, BlockID block) {
            size_t idx = index(x, y, z);
            data[idx] = (block << BLOCK_SHIFT) | (data[idx] & LAYER_MASK);
        }

        inline void setBlockOnly(const glm::ivec3& pos, BlockID block) {
            setBlockOnly(pos.x, pos.y, pos.z, block);
        }

        // Clear voxel (set to 0)
        inline void clear(int x, int y, int z) { data[index(x, y, z)] = 0; }

        inline void clear(const glm::ivec3& pos) { clear(pos.x, pos.y, pos.z); }

        // Check if voxel is on specific layer
        inline bool isLayer(int x, int y, int z, Layer layer) const {
            return getLayer(x, y, z) == layer;
        }

        inline bool isLayer(const glm::ivec3& pos, Layer layer) const {
            return getLayer(pos) == layer;
        }

        // Check if voxel is empty (both block and layer are 0)
        inline bool isEmpty(int x, int y, int z) const { return data[index(x, y, z)] == 0; }

        inline bool isEmpty(const glm::ivec3& pos) const { return isEmpty(pos.x, pos.y, pos.z); }
    };

}  // namespace engine