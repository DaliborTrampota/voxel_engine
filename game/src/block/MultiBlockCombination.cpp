#include "MultiBlockCombination.h"

#include <algorithm>

MultiBlockCombination::MultiBlockCombination(
    std::initializer_list<MultiBlockCombination::SubBlock> blocks
)
    : blocks(blocks) {}

bool MultiBlockCombination::contains(engine::BlockID blockID) const {
    return std::find_if(blocks.begin(), blocks.end(), [blockID](const SubBlock& subBlock) {
               return subBlock.blockID == blockID;
           }) != blocks.end();
}

bool MultiBlockCombination::canCombine(std::vector<engine::BlockID> blockIDs) const {
    int used = 0;
    for (const auto& block : blocks) {
        if (block.canRepeat) {
            for (const auto& blockID : blockIDs) {
                if (blockID == block.blockID) {
                    used++;
                    break;
                }
            }
        } else {
            int count = 0;
            for (const auto& blockID : blockIDs) {
                if (blockID == block.blockID) {
                    count++;
                }
            }
            if (count != 1) {
                return false;
            }
            used++;
        }
    }
    return used == blockIDs.size();
}