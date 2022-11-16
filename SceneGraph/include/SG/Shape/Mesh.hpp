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
#include <SG/Shape/Shape.hpp>
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Property.hpp>

#include <glm/ext/matrix_float4x4.hpp>
#include <map>
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Geometry;
class Material;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Mesh : public Inherit<Shape, Mesh> {
public:
    using GeometryMap = std::map<std::shared_ptr<Geometry>, std::shared_ptr<Material>>;
    class Skin;
    PROPERTY(std::shared_ptr<Skin>, Skin, nullptr);
    PROPERTY(glm::mat4, GeometryTransform, 1);
    PROPERTY(GeometryMap, Geometries, );

public:
    inline Mesh() : Inherit() {}
    inline Mesh(const std::string& a_Name) : Inherit(a_Name) {};

    /** Adds the Geometry to Geometrys list */
    inline void AddGeometry(std::shared_ptr<Geometry> a_Geometry, std::shared_ptr<Material> a_Material) {
        GetGeometries()[a_Geometry] = a_Material;
    }
    inline auto& GetGeometryMaterial(std::shared_ptr<Geometry> a_Geometry) const {
        return GetGeometries().at(a_Geometry);
    }
    inline void SetGeometryMaterial(std::shared_ptr<Geometry> a_Geometry, std::shared_ptr<Material> a_Material) {
        GetGeometries().at(a_Geometry) = a_Material;
    }
};
}
