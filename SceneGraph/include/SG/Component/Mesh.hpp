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
#include <SG/Core/Object.hpp>
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Property.hpp>

#include <glm/ext/matrix_float4x4.hpp>
#include <map>
#include <memory>
#include <string>

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
class Mesh : public Inherit<Object, Mesh> {
public:
    using GeometryMap = std::map<std::shared_ptr<Primitive>, std::shared_ptr<Material>>;
    PROPERTY(glm::mat4, GeometryTransform, 1);
    PROPERTY(GeometryMap, Primitives, );

public:
    inline Mesh() : Inherit() {}
    inline Mesh(const std::string& a_Name) : Inherit(a_Name) {};
    ~Mesh() {}

    /** Adds the Geometry to Geometrys list */
    inline void AddPrimitive(std::shared_ptr<Primitive> a_Geometry, std::shared_ptr<Material> a_Material) {
        GetPrimitives()[a_Geometry] = a_Material;
    }
    inline auto& GetPrimitiveMaterial(std::shared_ptr<Primitive> a_Geometry) const {
        return GetPrimitives().at(a_Geometry);
    }
    inline void SetGeometryMaterial(std::shared_ptr<Primitive> a_Geometry, std::shared_ptr<Material> a_Material) {
        GetPrimitives().at(a_Geometry) = a_Material;
    }
};
}
