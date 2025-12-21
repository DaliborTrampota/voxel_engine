#include "Geometries.h"

#include <cstdint>
#include <fstream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <block/Face.h>
#include <block/Geometry.h>
#include <data/Registry.h>
#include <data/RegistryManager.h>


#define OGT_VOX_IMPLEMENTATION
#include <tools/ogt_vox.h>

#define OGT_VOXEL_MESHIFY_IMPLEMENTATION
#include <tools/ogt_vox_meshify.h>

#include "MyRegistryManager.h"

using namespace engine;


namespace {

    glm::vec3 toGlmVec3(const ogt_mesh_vec3& vec) {
        return glm::vec3(vec.x / 16.0f, vec.z / 16.0f, vec.y / 16.0f);
    }

    std::array<glm::vec2, 3> calculateUVs(
        const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& n
    ) {
        // Find the dominant axis of the normal (which plane the triangle lies on)
        glm::vec3 absNormal = glm::abs(n);

        auto projectToUV = [](const glm::vec3& p, int uAxis, int vAxis) -> glm::vec2 {
            return glm::vec2(p[uAxis], p[vAxis]);
        };

        glm::vec2 uv0, uv1, uv2;

        if (absNormal.x > absNormal.y && absNormal.x > absNormal.z) {
            // Normal points along X axis, use YZ plane
            uv0 = projectToUV(p0, 1, 2);  // Y, Z
            uv1 = projectToUV(p1, 1, 2);
            uv2 = projectToUV(p2, 1, 2);
        } else if (absNormal.y > absNormal.z) {
            // Normal points along Y axis, use XZ plane
            uv0 = projectToUV(p0, 0, 2);  // X, Z
            uv1 = projectToUV(p1, 0, 2);
            uv2 = projectToUV(p2, 0, 2);
        } else {
            // Normal points along Z axis, use XY plane
            uv0 = projectToUV(p0, 0, 1);  // X, Y
            uv1 = projectToUV(p1, 0, 1);
            uv2 = projectToUV(p2, 0, 1);
        }

        return {uv0, uv1, uv2};
    }

    Geometry loadVOX(const std::string& path, int cubesPerSide = 16) {
        std::ifstream file(path, std::ios::binary);
        std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(file), {});
        const ogt_vox_scene* scene = ogt_vox_read_scene(buffer.data(), buffer.size());

        static ogt_voxel_meshify_context ctx{nullptr, nullptr, nullptr};

        const ogt_vox_model* model = scene->models[0];
        ogt_mesh* mesh = ogt_mesh_from_paletted_voxels_greedy(
            &ctx,
            model->voxel_data,
            model->size_x,
            model->size_y,
            model->size_z,
            reinterpret_cast<const ogt_mesh_rgba*>(scene->palette.color)
        );

        std::vector<Face> faces;

        for (int i = 0; i < mesh->index_count; i += 3) {
            int index0 = mesh->indices[i];
            int index1 = mesh->indices[i + 1];
            int index2 = mesh->indices[i + 2];

            glm::vec3 p0 = toGlmVec3(mesh->vertices[index0].pos);
            glm::vec3 p1 = toGlmVec3(mesh->vertices[index2].pos);
            glm::vec3 p2 = toGlmVec3(mesh->vertices[index1].pos);

            // Calculate normal from triangle edges
            glm::vec3 edge1 = p1 - p0;
            glm::vec3 edge2 = p2 - p0;
            glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

            // Calculate UVs based on the plane the triangle lies on
            auto uvs = calculateUVs(p0, p1, p2, normal);

            faces.push_back(Face::TriangleFace(FaceTag::Side, p0, p1, p2, uvs[0], uvs[1], uvs[2]));
        }


        ogt_mesh_destroy(&ctx, mesh);
        ogt_vox_destroy_scene(scene);
        return Geometry(faces);
    }
}  // namespace

void RegisterGeometries() {
    // clang-format off
    RegistryManager::GeometryRegistryT& geometries = MyRegistryManager::Geometries();

    geometries.add(Geometry::Cube(), "cube");
    geometries.add(Geometry::Cylinder(), "cylinder");
    geometries.add(Pyramid(), "pyramid");
    geometries.add(Log(4), "log_4");
    geometries.add(Log(6), "log_6");
    geometries.add(Log(8), "log_8");
    geometries.add(Log(10), "log_10");
    geometries.add(Log(12), "log_12");
    geometries.add(Log(14), "log_14");

    Geometry conn = loadVOX("resources/models/oak_log_connector.vox");
    conn.rotate({0, 1, 0}, glm::pi<float>() / 2);
    geometries.add(conn, "oak_log_connector");
    geometries.add(loadVOX("resources/models/oak_log_branch.vox"), "oak_log_branch");
    geometries.add(loadVOX("resources/models/cube.vox"), "cube_vox");

    geometries.add(Vegetation(VegetationPattern::Cross, .5f, 1.f), "vegetation_cross");
    geometries.add(Vegetation(VegetationPattern::Square, .5f, 1.f), "vegetation_square");
    geometries.add(Vegetation(VegetationPattern::Hash, .5f, 1.f), "vegetation_hash");

    // clang-format on
}

Geometry CreateRotatedGeometry(Geometry geo, glm::vec3 axis, float angle) {
    geo.rotate(axis, angle);
    return geo;
}


Geometry Pyramid() {
    std::vector<Face> faces;
    faces.push_back(
        Face::SquareFace(
            FaceTag::Bottom,
            glm::vec3(0, 0, 0),
            glm::vec3(1, 0, 1),
            glm::vec3(0, -1, 0),
            glm::vec2(0, 0),
            glm::vec2(1, 1)
        )
    );
    faces.push_back(
        Face::TriangleFace(
            FaceTag::Side,
            glm::vec3(0, 0, 0),
            glm::vec3(0, 0, 1),
            glm::vec3(0.5f, 1, 0.5f),
            glm::vec2(0, 0),
            glm::vec2(1, 0),
            glm::vec2(0.5f, 1)
        )
    );
    faces.push_back(
        Face::TriangleFace(
            FaceTag::Side,
            glm::vec3(0, 0, 1),
            glm::vec3(1, 0, 1),
            glm::vec3(0.5f, 1, 0.5f),
            glm::vec2(0, 0),
            glm::vec2(1, 0),
            glm::vec2(0.5f, 1)
        )
    );
    faces.push_back(
        Face::TriangleFace(
            FaceTag::Side,
            glm::vec3(1, 0, 0),
            glm::vec3(0, 0, 0),
            glm::vec3(0.5f, 1, 0.5f),
            glm::vec2(0, 0),
            glm::vec2(1, 0),
            glm::vec2(0.5f, 1)
        )
    );
    faces.push_back(
        Face::TriangleFace(
            FaceTag::Side,
            glm::vec3(1, 0, 1),
            glm::vec3(1, 0, 0),
            glm::vec3(0.5f, 1, 0.5f),
            glm::vec2(0, 0),
            glm::vec2(1, 0),
            glm::vec2(0.5f, 1)
        )
    );
    return Geometry(faces);
}

Geometry Log(int width) {
    std::vector<Face> faces;
    float unit = 1.f / 16.f;
    float unitWidth = width * unit;
    float offset = (1.f - unitWidth) / 2;

    glm::vec2 uvStart{offset, 0};            // = glm::vec2(offset, offset);
    glm::vec2 uvEnd{offset + unitWidth, 1};  // = glm::vec2(offset + unitWidth, offset + unitWidth);

    glm::vec3 base = glm::vec3(offset, 0, offset);
    glm::vec3 top = glm::vec3(offset, 1, offset);

    glm::vec3 add = {unitWidth, 0, unitWidth};

    // -Z
    faces.push_back(
        Face::RectangleFace(
            FaceTag::Side,
            base,
            top,  // + add * glm::vec3(1, 0, 0),
            unitWidth,
            glm::vec3(0, 0, -1),
            uvStart,
            uvEnd
        )
    );

    // X
    faces.push_back(
        Face::RectangleFace(
            FaceTag::Side,
            base + add * glm::vec3(1, 0, 0),
            top + add * glm::vec3(1, 0, 0),  //1
            unitWidth,
            glm::vec3(1, 0, 0),
            uvStart,
            uvEnd
        )
    );

    // Z
    faces.push_back(
        Face::RectangleFace(
            FaceTag::Side,
            base + add * glm::vec3(1, 0, 1),
            top + add * glm::vec3(1, 0, 1),  //x1
            unitWidth,
            glm::vec3(0, 0, 1),
            uvStart,
            uvEnd
        )
    );

    // X-
    faces.push_back(
        Face::RectangleFace(
            FaceTag::Side,
            base + add * glm::vec3(0, 0, 1),
            top + add * glm::vec3(0, 0, 1),  // nothing
            unitWidth,
            glm::vec3(-1, 0, 0),
            uvStart,
            uvEnd
        )
    );


    Face topFace = Face::SquareFace(
        FaceTag::Top,
        glm::vec3(offset, 1, offset),
        glm::vec3(offset + unitWidth, 1, offset + unitWidth),
        glm::vec3(0, 1, 0),
        glm::vec2(0, 0),
        glm::vec2(1, 1)
    );
    topFace.cull = false;
    faces.push_back(topFace);
    Face bottomFace = Face::SquareFace(
        FaceTag::Bottom,
        glm::vec3(offset, 0, offset),
        glm::vec3(offset + unitWidth, 0, offset + unitWidth),
        glm::vec3(0, -1, 0),
        glm::vec2(0, 0),
        glm::vec2(1, 1)
    );
    bottomFace.cull = true;
    faces.push_back(bottomFace);

    return Geometry(faces);
}


Geometry Vegetation(VegetationPattern pattern, float w, float h) {
    std::vector<Face> faces;

    Face rect = Face::RectangleFace(
        FaceTag::Side,
        glm::vec3(w, 0, 0),
        glm::vec3(w, h, 0),
        w,
        glm::vec3(0, 0, 1),
        glm::vec2(0, 0),
        glm::vec2(1, 1)
    );
    rect.setDoubleSided(true);

    switch (pattern) {
        case VegetationPattern::Cross:
            rect.translate(glm::vec3(0.5f - w / 2.f, 0.f, 0.5f));
            rect.rotate({0, 1, 0}, glm::pi<float>() / 4);
            faces.push_back(rect);
            rect.rotate({0, 1, 0}, glm::pi<float>() / 2);
            faces.push_back(rect);
            break;

        case VegetationPattern::Square:
            rect.translate(glm::vec3(0.5f - w / 2.f, 0.f, 0.5f - w / 2.f));
            for (int i = 0; i < 4; i++) {
                faces.push_back(rect);
                rect.rotate({0, 1, 0}, glm::pi<float>() / 2);
            }
            break;

        case VegetationPattern::Hash:
            rect.translate(glm::vec3(0.5f - w / 2.f, 0, 0.5f - w / 2.f + w * 0.2f));
            for (int i = 0; i < 4; i++) {
                faces.push_back(rect);
                rect.rotate({0, 1, 0}, glm::pi<float>() / 2);
            }
            break;
    }
    return Geometry(faces);
}