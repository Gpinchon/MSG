#include <MSG/Mesh.hpp>
#include <MSG/Mesh/Primitive.hpp>
#include <MSG/ShapeGenerator/Capsule.hpp>

#include <glm/gtc/constants.hpp>

namespace Msg::ShapeGenerator {
std::vector<glm::vec3> getUnitCircleVertices(int sectorCount)
{
    float sectorStep = 2 * glm::pi<float>() / float(sectorCount);
    float sectorAngle; // radian

    std::vector<glm::vec3> unitCircleVertices;
    for (int i = 0; i <= sectorCount; ++i) {
        sectorAngle = i * sectorStep;
        unitCircleVertices.push_back({ cos(sectorAngle), sin(sectorAngle), 0 });
    }
    return unitCircleVertices;
}
}

Msg::MeshPrimitive Msg::ShapeGenerator::CreateCapsulePrimitive(float height, float radius, int sectorCount, int stackCount)
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<unsigned> indices;

    stackCount += stackCount % 2 ? 0 : 1; // We need an odd number of stacks
    float sectorStep = 2 * glm::pi<float>() / float(sectorCount);
    float stackStep  = glm::pi<float>() / float(stackCount);
    for (auto stack = 0; stack <= stackCount; ++stack) {
        float heightOffset = stack > stackCount / 2 ? -height / 2.f : height / 2.f;
        auto stackAngle    = glm::pi<float>() / 2.f - stack * stackStep;
        auto xy            = radius * cosf(stackAngle);
        auto y             = radius * sinf(stackAngle);
        for (auto sector = 0; sector <= sectorCount; ++sector) {
            auto sectorAngle = sector * sectorStep;
            auto x           = xy * cosf(sectorAngle);
            auto z           = xy * sinf(sectorAngle);
            auto s           = float(sector) / float(sectorCount);
            auto t           = float(stack) / float(stackCount);
            vertices.push_back({ x, y + heightOffset, z });
            normals.push_back(normalize(glm::vec3(x, y, z)));
            texCoords.push_back({ s, t });
        }
    }
    int k1, k2;
    for (int i = 0; i < stackCount; ++i) {
        k1 = i * (sectorCount + 1); // beginning of current stack
        k2 = k1 + sectorCount + 1; // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k1 + 1);
                indices.push_back(k2);
            }

            // k1+1 => k2 => k2+1
            if (i != (stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2 + 1);
                indices.push_back(k2);
            }
        }
    }
    return { vertices, normals, texCoords, indices };
}

Msg::Mesh Msg::ShapeGenerator::CreateCapsuleMesh(float a_Heigth, float a_Radius, int a_SectorCount, int a_HeightSubdivision)
{
    auto primitive = std::make_shared<MeshPrimitive>(CreateCapsulePrimitive(a_Heigth, a_Radius, a_SectorCount, a_HeightSubdivision));
    Mesh m;
    m.emplace_back()[primitive] = 0;
    m.ComputeBoundingVolume();
    return m;
}
