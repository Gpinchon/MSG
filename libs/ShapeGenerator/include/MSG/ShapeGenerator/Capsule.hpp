#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Mesh.hpp>

#include <memory>
#include <string>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG::ShapeGenerator {
Mesh CreateCapsuleMesh(const std::string& name, float heigth, float radius, int sectorCount = 32, int heightSubdivision = 10);
MeshPrimitive CreateCapsulePrimitive(const std::string& name, float heigth, float radius, int sectorCount = 32, int heightSubdivision = 10);
}
