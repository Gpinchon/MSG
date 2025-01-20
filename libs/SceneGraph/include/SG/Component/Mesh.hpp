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
#include <SG/Component/BoundingVolume.hpp>
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
namespace MSG::SG {
class Primitive;
class Material;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG::SG::Component {
using MeshLodGeometryMap = std::map<std::shared_ptr<Primitive>, std::shared_ptr<Material>>;
class MeshLod : public MeshLodGeometryMap {
public:
    float screenCoverage = 1;
};
using MeshLods = std::vector<MeshLod>;
class Mesh : public MeshLods {
public:
    using MeshLods::MeshLods;
    template<typename...Args>
    Mesh(const std::string& a_Name, Args... a_Args);
    void ComputeBoundingVolume();
    std::vector<std::shared_ptr<Primitive>> GetPrimitives(const uint8_t& a_Lod = 0) const;
    std::vector<std::shared_ptr<Material>> GetMaterials(const uint8_t& a_Lod = 0) const;
    Name name;
    glm::mat4 geometryTransform { 1 };
    Component::BoundingVolume boundingVolume; // bounding volume for the base level
};

template <typename... Args>
inline Mesh::Mesh(const std::string& a_Name, Args... a_Args)
    : MeshLods(a_Args...)
{
    name = a_Name;
}

inline std::vector<std::shared_ptr<Primitive>> Mesh::GetPrimitives(const uint8_t& a_Lod) const
{
    std::vector<std::shared_ptr<Primitive>> prim;
    prim.reserve(at(a_Lod).size());
    for (auto& [primitive, material] : at(a_Lod))
        prim.emplace_back(primitive);
    return prim;
}

inline std::vector<std::shared_ptr<Material>> Mesh::GetMaterials(const uint8_t& a_Lod) const
{
    std::vector<std::shared_ptr<Material>> mat;
    mat.reserve(at(a_Lod).size());
    for (auto& [primitive, material] : at(a_Lod))
        mat.emplace_back(material);
    std::sort(mat.begin(), mat.end());
    auto last = std::unique(mat.begin(), mat.end());
    mat.erase(last, mat.end());
    return mat;
}
}
