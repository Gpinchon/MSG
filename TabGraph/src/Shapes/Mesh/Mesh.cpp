/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-12 16:31:00
*/

#include <Shapes/Mesh/Mesh.hpp>
#include <Material/Standard.hpp>
#include <Shapes/Geometry.hpp>

#if RENDERINGAPI == OpenGL
#include <Driver/OpenGL/Renderer/Shapes/MeshRenderer.hpp>
#endif

#include <memory> // for shared_ptr
#include <stddef.h> // for size_t

namespace TabGraph::Shapes {
size_t meshNbr(0);

Mesh::Mesh()
    : Mesh("Mesh_" + std::to_string(meshNbr))
{
    meshNbr++;
}

Mesh::Mesh(const std::string& name)
    : Inherit(name)
{
    _renderer.reset(new Renderer::MeshRenderer(*this));
    meshNbr++;
}

Mesh::Mesh(const Mesh& other)
    : Inherit(other)
    , _geometries(other._geometries)
{
    _renderer.reset(new Renderer::MeshRenderer(*this));
    SetGeometryTransform(other.GetGeometryTransform());
    meshNbr++;
}

Mesh::~Mesh()
{
}

void Mesh::AddGeometry(std::shared_ptr<Geometry> geometry, std::shared_ptr<Material::Standard> material)
{
    return SetGeometryMaterial(geometry, material);
}

std::shared_ptr<Material::Standard> Mesh::GetGeometryMaterial(std::shared_ptr<Geometry> geometry) const
{
    auto itr { _geometries.find(geometry) };
    return itr != _geometries.end() ? itr->second : nullptr;
}

void Mesh::SetGeometryMaterial(std::shared_ptr<Geometry> geometry, std::shared_ptr<Material::Standard> material)
{
    _geometries[geometry] = material;
}

void Mesh::Load()
{
    if (GetLoaded())
        return;
    _loaded = true;
}

}
