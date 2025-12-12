#pragma once

#include <block/Block.h>
#include <vector>


struct MultiBlockCombination {
    struct SubBlock {
        engine::BlockID blockID;
        bool canRepeat;
    };

    MultiBlockCombination(std::initializer_list<MultiBlockCombination::SubBlock> blocks);

    engine::BlockID getID() const;
    bool contains(engine::BlockID blockID) const;
    bool canCombine(std::vector<engine::BlockID> blocks) const;

    std::vector<SubBlock> blocks;
};