/*
 * @Author: gpinchon
 * @Date:   2020-07-28 00:01:40
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2020-08-09 11:38:44
 */

#include <SG/Component/Mesh.hpp>
#include <SG/Core/Material.hpp>
#include <SG/Core/Primitive.hpp>
#include <Tools/Pi.hpp>

#include <algorithm>
#include <glm/vec3.hpp>
#include <vector>

namespace TabGraph::SG::Sphere {
std::map<int64_t, unsigned> middlePointIndexCache;

glm::vec2 FindUV(const glm::vec3& normal)
{
    glm::vec2 uv;
    const float& x    = normal.x;
    const float& y    = normal.y;
    const float& z    = normal.z;
    float normalisedX = 0;
    float normalisedZ = -1;
    if (((x * x) + (z * z)) > 0) {
        normalisedX = sqrt((x * x) / ((x * x) + (z * z)));
        if (x < 0) {
            normalisedX = -normalisedX;
        }
        normalisedZ = sqrt((z * z) / ((x * x) + (z * z)));
        if (z < 0) {
            normalisedZ = -normalisedZ;
        }
    }
    if (normalisedZ == 0) {
        uv.x = ((normalisedX * M_PIf) / 2);
    } else {
        uv.x = atan(normalisedX / normalisedZ);
        if (normalisedX < 0) {
            uv.x = M_PIf - uv.x;
        }
        if (normalisedZ < 0) {
            uv.x += M_PIf;
        }
    }
    if (uv.x < 0) {
        uv.x += 2 * M_PIf;
    }
    uv.x /= 2 * M_PIf;
    uv.y = (-y + 1) / 2;
    return uv;
}

auto getMiddlePoint(unsigned p1, unsigned p2, std::vector<glm::vec3>& positions)
{
    // first check if we have it already
    bool firstIsSmaller  = p1 < p2;
    int64_t smallerIndex = firstIsSmaller ? p1 : p2;
    int64_t greaterIndex = firstIsSmaller ? p2 : p1;
    auto key             = (smallerIndex << 32) + greaterIndex;

    if (middlePointIndexCache.count(key) > 0) {
        return middlePointIndexCache[key];
    }

    // not in cache, calculate it
    glm::vec3 point1 = positions.at(p1);
    glm::vec3 point2 = positions.at(p2);
    glm::vec3 middle = glm::vec3(
        (point1.x + point2.x) / 2.0,
        (point1.y + point2.y) / 2.0,
        (point1.z + point2.z) / 2.0);

    // add vertex makes sure point is on unit sphere
    positions.push_back(normalize(middle));
    auto i = unsigned(positions.size() - 1);

    // store it, return index
    middlePointIndexCache[key] = i;
    return i;
}

Primitive CreatePrimitive(const std::string& a_Name, float a_Radius, unsigned a_Subdivision)
{
    const float t = (1.f + std::sqrt(5.f)) / 2.f;

    std::vector<glm::vec3> vertice;
    vertice.push_back(normalize(glm::vec3(-1.0, t, 0.0)));
    vertice.push_back(normalize(glm::vec3(1.0, t, 0.0)));
    vertice.push_back(normalize(glm::vec3(-1.0, -t, 0.0)));
    vertice.push_back(normalize(glm::vec3(1.0, -t, 0.0)));
    vertice.push_back(normalize(glm::vec3(0.0, -1.0, t)));
    vertice.push_back(normalize(glm::vec3(0.0, 1.0, t)));
    vertice.push_back(normalize(glm::vec3(0.0, -1.0, -t)));
    vertice.push_back(normalize(glm::vec3(0.0, 1.0, -t)));
    vertice.push_back(normalize(glm::vec3(t, 0.0, -1.0)));
    vertice.push_back(normalize(glm::vec3(t, 0.0, 1.0)));
    vertice.push_back(normalize(glm::vec3(-t, 0.0, -1.0)));
    vertice.push_back(normalize(glm::vec3(-t, 0.0, 1.0)));

    std::vector<std::array<unsigned, 3>> faces;
    // 5 faces around point 0
    faces.push_back({ 0, 11, 5 });
    faces.push_back({ 0, 5, 1 });
    faces.push_back({ 0, 1, 7 });
    faces.push_back({ 0, 7, 10 });
    faces.push_back({ 0, 10, 11 });

    // 5 adjacent faces
    faces.push_back({ 1, 5, 9 });
    faces.push_back({ 5, 11, 4 });
    faces.push_back({ 11, 10, 2 });
    faces.push_back({ 10, 7, 6 });
    faces.push_back({ 7, 1, 8 });

    // 5 faces around point 3
    faces.push_back({ 3, 9, 4 });
    faces.push_back({ 3, 4, 2 });
    faces.push_back({ 3, 2, 6 });
    faces.push_back({ 3, 6, 8 });
    faces.push_back({ 3, 8, 9 });

    // 5 adjacent faces
    faces.push_back({ 4, 9, 5 });
    faces.push_back({ 2, 4, 11 });
    faces.push_back({ 6, 2, 10 });
    faces.push_back({ 8, 6, 7 });
    faces.push_back({ 9, 8, 1 });
    for (unsigned i = 0; i < a_Subdivision; ++i) {
        std::vector<std::array<unsigned, 3>> faces2;
        for (const auto& tri : faces) {
            // replace triangle by 4 triangles
            auto a = getMiddlePoint(tri[0], tri[1], vertice);
            auto b = getMiddlePoint(tri[1], tri[2], vertice);
            auto c = getMiddlePoint(tri[2], tri[0], vertice);

            faces2.push_back({ tri[0], a, c });
            faces2.push_back({ tri[1], b, a });
            faces2.push_back({ tri[2], c, b });
            faces2.push_back({ a, b, c });
        }
        faces = faces2;
    }
    middlePointIndexCache.clear();
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    for (auto& v : vertice) {
        normals.push_back(v);
        texCoords.push_back(FindUV(v));
        v *= a_Radius;
    }
    std::vector<unsigned> indice;
    for (const auto& tri : faces) {
        indice.push_back(tri[0]);
        indice.push_back(tri[1]);
        indice.push_back(tri[2]);
    }
    Primitive primitive(vertice, normals, texCoords, indice);
    primitive.GenerateTangents();
    primitive.SetName(a_Name);
    return primitive;
}

Component::Mesh CreateMesh(const std::string& a_Name, float a_Radius, unsigned a_Subdivision)
{
    Component::Mesh m;
    m.name                                                                                                     = a_Name;
    m.primitives[std::make_shared<Primitive>(CreatePrimitive(a_Name + "_Primitive", a_Radius, a_Subdivision))] = std::make_shared<Material>(a_Name + "_Material");
    return m;
}
}
