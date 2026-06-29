#pragma once

#include <array>
#include <cassert>

#include "../Globals.h"
#include "block/Face.h"
#include "block/Side.h"

namespace engine::data {

    static_assert(
        static_cast<int>(Side::North) == static_cast<int>(FaceTag::Front),
        "Side::North must be equal to FaceTag::Front"
    );
    static_assert(
        static_cast<int>(Side::West) == static_cast<int>(FaceTag::Right),
        "Side::West must be equal to FaceTag::Right"
    );
    static_assert(
        static_cast<int>(Side::South) == static_cast<int>(FaceTag::Back),
        "Side::South must be equal to FaceTag::Back"
    );
    static_assert(
        static_cast<int>(Side::East) == static_cast<int>(FaceTag::Left),
        "Side::East must be equal to FaceTag::Left"
    );
    static_assert(
        static_cast<int>(Side::Up) == static_cast<int>(FaceTag::Top),
        "Side::Up must be equal to FaceTag::Top"
    );
    static_assert(
        static_cast<int>(Side::Down) == static_cast<int>(FaceTag::Bottom),
        "Side::Down must be equal to FaceTag::Bottom"
    );

    template <typename T, size_t row, size_t col>
    using array2d = std::array<std::array<T, col>, row>;

    constexpr float scale = 1.0f;

    constexpr std::array<glm::vec3, 8> vertices = {
        glm::vec3(0, 0, 0),  // origin
        glm::vec3(1, 0, 0),  // x down
        glm::vec3(1, 1, 0),  // x up
        glm::vec3(0, 1, 0),  // origin up
        glm::vec3(0, 1, 1),  // z up
        glm::vec3(1, 1, 1),  // opposite origin
        glm::vec3(1, 0, 1),  // x z down
        glm::vec3(0, 0, 1)   // z down
    };

    // constexpr array2d<int, 6, 4> faces_indexed = {{
    //     {0, 3, 4, 3},  // north
    //     {3, 2, 5, 4},  // west
    //     {4, 5, 6, 7},  // south
    //     {7, 6, 1, 0},  // east
    //     {6, 5, 2, 1},  // up
    //     {7, 0, 3, 4}   // down
    // }};

    constexpr array2d<int, 6, 6> faces = {{
        {4, 7, 5, 6, 5, 7},  // north
        {3, 0, 4, 7, 4, 0},  // west
        {0, 3, 1, 2, 1, 3},  // south
        {5, 6, 2, 1, 2, 6},  // east
        {3, 4, 2, 5, 2, 4},  // up
        {7, 0, 6, 1, 6, 0},  // down
    }};

    const std::array<Face, 6> f_faces = {
        Face::SquareFace(
            FaceTag::Front, vertices[6], vertices[4], NORTH, glm::vec2(0, 0), glm::vec2(1, 1)
        ),  // North
        Face::SquareFace(
            FaceTag::Right, vertices[7], vertices[3], -EAST, glm::vec2(0, 0), glm::vec2(1, 1)
        ),  // West
        Face::SquareFace(
            FaceTag::Back, vertices[0], vertices[2], -NORTH, glm::vec2(0, 0), glm::vec2(1, 1)
        ),  // South
        Face::SquareFace(
            FaceTag::Left, vertices[1], vertices[5], EAST, glm::vec2(0, 0), glm::vec2(1, 1)
        ),  // East
        Face::SquareFace(
            FaceTag::Top, vertices[2], vertices[4], UP, glm::vec2(0, 0), glm::vec2(1, 1)
        ),  // Up
        Face::SquareFace(
            FaceTag::Bottom, vertices[0], vertices[6], -UP, glm::vec2(0, 0), glm::vec2(1, 1)
        )  // Down
    };

    constexpr std::array<glm::vec3, 6> normals = {
        glm::vec3(NORTH),   // north
        glm::vec3(-EAST),   // west
        glm::vec3(-NORTH),  // south
        glm::vec3(EAST),    // east
        glm::vec3(UP),      // up
        glm::vec3(-UP)      // down
    };

    // Per-face UVs for Box(), one row per face matching `faces` above (north, west,
    // south, east, up, down). Unlike the single `uvs` table, these account for each
    // face's distinct vertex winding so every face is upright and consistent with the
    // SquareFace/Cube convention (start->uvEnd, end->uvStart).
    constexpr array2d<glm::vec2, 6, 6> box_uvs = {{
        // north (+Z)
        {glm::vec2(0, 0),
         glm::vec2(0, 1),
         glm::vec2(1, 0),
         glm::vec2(1, 1),
         glm::vec2(1, 0),
         glm::vec2(0, 1)},
        // west (-X)
        {glm::vec2(0, 0),
         glm::vec2(0, 1),
         glm::vec2(1, 0),
         glm::vec2(1, 1),
         glm::vec2(1, 0),
         glm::vec2(0, 1)},
        // south (-Z)
        {glm::vec2(1, 1),
         glm::vec2(1, 0),
         glm::vec2(0, 1),
         glm::vec2(0, 0),
         glm::vec2(0, 1),
         glm::vec2(1, 0)},
        // east (+X)
        {glm::vec2(0, 0),
         glm::vec2(0, 1),
         glm::vec2(1, 0),
         glm::vec2(1, 1),
         glm::vec2(1, 0),
         glm::vec2(0, 1)},
        // up (+Y)
        {glm::vec2(1, 0),
         glm::vec2(0, 0),
         glm::vec2(1, 1),
         glm::vec2(0, 1),
         glm::vec2(1, 1),
         glm::vec2(0, 0)},
        // down (-Y)
        {glm::vec2(0, 1),
         glm::vec2(1, 1),
         glm::vec2(0, 0),
         glm::vec2(1, 0),
         glm::vec2(0, 0),
         glm::vec2(1, 1)},
    }};

    constexpr std::array<glm::ivec2, 6> axisForSide = {
        glm::ivec2(2, 1),  // north
        glm::ivec2(0, 1),  // west
        glm::ivec2(2, 1),  // south
        glm::ivec2(0, 1),  // east
        glm::ivec2(2, 0),  // up
        glm::ivec2(0, 2)   // down
    };

}  // namespace engine::data