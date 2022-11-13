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
#include <Shapes/Shape.hpp>
#include <Core/Inherit.hpp>
#include <Core/Property.hpp>

#include <glm/ext/matrix_float4x4.hpp>
#include <map>
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Shapes {
class Geometry;
}
namespace Material {
class Standard;
}
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Shapes {
class Mesh : public TabGraph::Core::Inherit<Shape, Mesh> {
public:
    class Skin;
    PROPERTY(std::shared_ptr<Skin>, Skin, nullptr);
public:
    
    Mesh();
    Mesh(const std::string& name);
    Mesh(const Mesh& other);
    ~Mesh();

    /** Adds the Geometry to Geometrys list */
    void AddGeometry(std::shared_ptr<Geometry> geometry, std::shared_ptr<Material::Standard> material);
    std::shared_ptr<Material::Standard> GetGeometryMaterial(std::shared_ptr<Geometry> geometry) const;
    void SetGeometryMaterial(std::shared_ptr<Geometry> geometry, std::shared_ptr<Material::Standard> material);

    const auto& GetGeometries() const
    {
        return _geometries;
    }

    virtual void Load();
    inline bool GetLoaded()
    {
        return _loaded;
    }
    inline auto& GetGeometryTransform() const
    {
        return _geometryTransform;
    }
    inline void SetGeometryTransform(const glm::mat4& transform)
    {
        _geometryTransform = transform;
    }

private:
    std::map<std::shared_ptr<Geometry>, std::shared_ptr<Material::Standard>> _geometries;
    bool _loaded { false };
    glm::mat4 _geometryTransform { 1 };
};
}
