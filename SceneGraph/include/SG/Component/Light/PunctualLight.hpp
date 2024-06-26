#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Component/Name.hpp>

#include <Tools/Pi.hpp>

#include <glm/vec3.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Component {
struct LightBase {
    glm::vec3 color { 1 };
    float intensity { 1 };
    float range { std::numeric_limits<float>::infinity() };
};

struct LightPoint : LightBase { };

struct LightSpot : LightBase {
    float innerConeAngle { 0 };
    float outerConeAngle { M_PI / 4.0 };
};

struct LightDirectional : LightBase {
    glm::vec3 halfSize { std::numeric_limits<float>::infinity() };
};

union LightData {
    explicit LightData(const LightPoint& a_Point)
        : point(a_Point) {};
    explicit LightData(const LightSpot& a_Spot)
        : spot(a_Spot) {};
    explicit LightData(const LightDirectional& a_Dir)
        : directional(a_Dir) {};
    LightBase base = {};
    LightPoint point;
    LightSpot spot;
    LightDirectional directional;
};

struct PunctualLight {
    enum class Type {
        Unknown = -1,
        Point,
        Spot,
        Directional,
        MaxValue
    };
    PunctualLight(const PunctualLight&) = default;
    // by default PunctualLight is a point light
    PunctualLight(const LightPoint& a_Data = {})
        : type(Type::Point)
        , data(a_Data)
    {
    }
    PunctualLight(const LightSpot& a_Data)
        : type(Type::Spot)
        , data(a_Data)
    {
    }
    PunctualLight(const LightDirectional& a_Data)
        : type(Type::Point)
        , data(a_Data)
    {
    }
    Name name;
    Type type;
    LightData data;
};
}
