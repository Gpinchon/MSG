#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Mesh.hpp>

#include <string>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG::ShapeGenerator {
Core::Mesh CreateSphereMesh(const std::string& a_Name, float a_Radius, unsigned a_Subdivision = 2);
Core::Primitive CreateSpherePrimitive(const std::string& a_Name, float a_Radius, unsigned a_Subdivision = 2);
};
