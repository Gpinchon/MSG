#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Mesh.hpp>

#include <memory>
#include <string>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG::ShapeGenerator {
Core::Mesh CreateCapsuleMesh(const std::string& name, float heigth, float radius, int sectorCount = 32, int heightSubdivision = 10);
Core::Primitive CreateCapsulePrimitive(const std::string& name, float heigth, float radius, int sectorCount = 32, int heightSubdivision = 10);
}
