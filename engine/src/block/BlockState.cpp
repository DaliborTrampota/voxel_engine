#include "BlockState.h"

using namespace engine;

BlockState BlockState::makeRotation(glm::vec3 facing) {
    BlockState state;
    state.m_facing = facing;
    return state;
}