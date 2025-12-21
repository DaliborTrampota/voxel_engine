#include "Algorithms.h"
#include "CoordUtils.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/common.hpp>


#include "block/Block.h"
#include "block/Face.h"
#include "block/Geometry.h"
#include "level/World.h"


using namespace engine;

namespace {

    constexpr float epsilon = 1e-9;

    float mod(float value, float modulus) {
        float result = glm::mod(value, modulus);
        return result < 0.0f ? result + modulus : result;
    }

    float intbound(float s, float ds) {
        float is = mod(ds < 0.0f ? -s : s, 1.0f);
        return (1.0f - is) / glm::abs(ds);
    }
}  // namespace

// TODO return the geometry face, not just normal of a cube
DDAResult engine::DDA(
    const World& world, glm::vec3 start, glm::vec3 direction, float length, UnaryPredicate pred
) noexcept {
    if (glm::all(glm::equal(direction, glm::vec3(0.0f)))) {
        return DDAResult{start, glm::vec3(0.0f), &Block::air(), 0.f, nullptr};
    }

    int x = glm::floor(start.x);
    int y = glm::floor(start.y);
    int z = glm::floor(start.z);

    float dx = direction.x;
    float dy = direction.y;
    float dz = direction.z;

    int stepX = glm::sign(dx);
    int stepY = glm::sign(dy);
    int stepZ = glm::sign(dz);

    float tMaxX = intbound(start.x, dx);
    float tMaxY = intbound(start.y, dy);
    float tMaxZ = intbound(start.z, dz);

    float tDeltaX = stepX / static_cast<float>(dx);
    float tDeltaY = stepY / static_cast<float>(dy);
    float tDeltaZ = stepZ / static_cast<float>(dz);

    glm::vec3 face(0.0f);

    length /= glm::sqrt(dx * dx + dy * dy + dz * dz);

    // TODO condition should be world bound check
    while (true) {
        glm::ivec3 pos = glm::ivec3(x, y, z);
        const Chunk* chunk = world.getChunk(extractChunkCoords(pos));
        const Block* block = chunk->getBlock(pos);
        //TODO check if we are in bounds
        if (pred(block)) {
            return DDAResult{glm::ivec3(x, y, z), face, block, 0.f, chunk};
        }

        if (tMaxX < tMaxY) {
            if (tMaxX < tMaxZ) {
                if (tMaxX > length)
                    break;
                x += stepX;
                tMaxX += tDeltaX;
                face = glm::vec3(-stepX, 0.0f, 0.0f);
            } else {
                if (tMaxZ > length)
                    break;
                z += stepZ;
                tMaxZ += tDeltaZ;
                face = glm::vec3(0.0f, 0.0f, -stepZ);
            }
        } else {
            if (tMaxY < tMaxZ) {
                if (tMaxY > length)
                    break;
                y += stepY;
                tMaxY += tDeltaY;
                face = glm::vec3(0.0f, -stepY, 0.0f);
            } else {
                if (tMaxZ > length)
                    break;
                z += stepZ;
                tMaxZ += tDeltaZ;
                face = glm::vec3(0.0f, 0.0f, -stepZ);
            }
        }
    }  // end while

    return DDAResult{{x, y, z}, face, &Block::air(), length, nullptr};
}


float engine::rayTriangleIntersection(Ray& ray, Triangle& t, float& uOut, float& vOut) noexcept {
    glm::vec3 e1 = t.p2 - t.p1;
    glm::vec3 e2 = t.p3 - t.p1;

    glm::vec3 p = glm::cross(ray.direction, e2);
    float det = glm::dot(e1, p);

    // TODO
#if CULL_BACKFACES
    if (det < epsilon) {
        return std::numeric_limits<float>::infinity();
    }
#else
    if (glm::abs(det) < epsilon) {
        return std::numeric_limits<float>::infinity();
    }
#endif
    float d_inv = 1.0 / det;
    glm::vec3 q = ray.origin - t.p1;
    float u = d_inv * glm::dot(q, p);
    if (u < 0.0 || u > 1.0)
        return std::numeric_limits<float>::infinity();

    glm::vec3 r = glm::cross(q, e1);
    float v = d_inv * glm::dot(ray.direction, r);
    if (v < 0.0 || u + v > 1.0)
        return std::numeric_limits<float>::infinity();

    uOut = u;
    vOut = v;
    return d_inv * glm::dot(e2, r);
}

const Face* engine::getAimedFace(Ray& ray, const Geometry& geometry) noexcept {
    struct {
        float minT = std::numeric_limits<float>::infinity();
        const Face* face = nullptr;
    } best;
    for (const auto& face : geometry.faces()) {
        for (int i = 0; i < face.vertices.size(); i += 3) {
            Triangle trig = {
                face.vertices[i].pos, face.vertices[i + 1].pos, face.vertices[i + 2].pos
            };
            float u, v;
            float t = rayTriangleIntersection(ray, trig, u, v);
            if (t < best.minT) {
                best.minT = t;
                best.face = &face;
            }
        }
    }
    return best.face;
}


glm::vec3 engine::rotatePoint(
    const glm::vec3& point, const glm::vec3& axis, float angle, const glm::vec3& center
) {
    glm::mat4 rotation = glm::translate(glm::mat4(1.0f), center) *
                         glm::rotate(glm::mat4(1.0f), angle, axis) *
                         glm::translate(glm::mat4(1.0f), -center);

    return glm::vec3(rotation * glm::vec4(point, 1.0f));
}

std::vector<glm::vec3> engine::rotatePoints(
    const std::vector<glm::vec3>& points, const glm::vec3& axis, float angle, const glm::vec3& center
) {
    glm::mat4 rotation = glm::translate(glm::mat4(1.0f), center) *
                         glm::rotate(glm::mat4(1.0f), angle, axis) *
                         glm::translate(glm::mat4(1.0f), -center);
    std::vector<glm::vec3> rotatedPoints;
    rotatedPoints.reserve(points.size());
    for (const auto& point : points) {
        rotatedPoints.emplace_back(rotation * glm::vec4(point, 1.0f));
    }
    return rotatedPoints;
}