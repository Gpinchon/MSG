#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/BoundingVolume.hpp>
#include <MSG/Core/Name.hpp>

#include <glm/ext/matrix_float4x4.hpp>

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace Msg {
class MeshPrimitive;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace Msg {
/** @brief Stores the material index to use inside a MaterialSet */
using MeshLodGeometryMap = std::map<std::shared_ptr<MeshPrimitive>, size_t>;
/** @brief Stores a level of detail of a mesh */
class MeshLod : public MeshLodGeometryMap {
public:
    float screenCoverage = 1;
};
using MeshLods = std::vector<MeshLod>;
class Mesh : public MeshLods {
public:
    using MeshLods::MeshLods;
    template <typename... Args>
    Mesh(const std::string& a_Name, Args... a_Args);
    void ComputeBoundingVolume();
    std::vector<std::shared_ptr<MeshPrimitive>> GetPrimitives(const uint8_t& a_Lod = 0) const;
    Core::Name name;
    glm::mat4 geometryTransform { 1 };
    BoundingVolume boundingVolume; // bounding volume for the base level
};

template <typename... Args>
inline Mesh::Mesh(const std::string& a_Name, Args... a_Args)
    : MeshLods(a_Args...)
{
    name = a_Name;
}

inline std::vector<std::shared_ptr<MeshPrimitive>> Mesh::GetPrimitives(const uint8_t& a_Lod) const
{
    std::vector<std::shared_ptr<MeshPrimitive>> prim;
    prim.reserve(at(a_Lod).size());
    for (auto& [primitive, material] : at(a_Lod))
        prim.emplace_back(primitive);
    return prim;
}
}
