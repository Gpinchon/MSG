/*
 * @Author: gpinchon
 * @Date:   2019-02-22 16:19:03
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-07-01 22:30:45
 */

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Component/Name.hpp>

#include <glm/ext/matrix_float4x4.hpp>

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Primitive;
class Material;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Component {
struct Mesh {
    using GeometryMap = std::map<std::shared_ptr<Primitive>, std::shared_ptr<Material>>;
    Mesh()            = default;
    Mesh(const std::string& a_Name);
    std::vector<std::shared_ptr<Primitive>> GetPrimitives() const;
    std::vector<std::shared_ptr<Material>> GetMaterials() const;
    Name name;
    glm::mat4 geometryTransform { 1 };
    GeometryMap primitives;
};

inline Mesh::Mesh(const std::string& a_Name)
    : name(a_Name)
{
}

inline std::vector<std::shared_ptr<Primitive>> Mesh::GetPrimitives() const
{
    std::vector<std::shared_ptr<Primitive>> prim;
    prim.reserve(primitives.size());
    for (auto& primitive : primitives)
        prim.emplace_back(primitive.first);
    return prim;
}

inline std::vector<std::shared_ptr<Material>> Mesh::GetMaterials() const
{
    std::vector<std::shared_ptr<Material>> mat;
    mat.reserve(primitives.size());
    for (auto& primitive : primitives)
        mat.emplace_back(primitive.second);
    std::sort(mat.begin(), mat.end());
    auto last = std::unique(mat.begin(), mat.end());
    mat.erase(last, mat.end());
    return mat;
}
}
