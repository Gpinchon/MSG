/*
* @Author: gpinchon
* @Date:   2021-06-26 23:41:58
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-24 21:29:00
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <memory>
#include <string>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Shapes {
class Mesh;
class Geometry;
}

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Shapes::Generators::Capsule {
std::shared_ptr<Mesh> CreateMesh(const std::string& name, float heigth, float radius, int sectorCount = 32, int heightSubdivision = 10);
std::shared_ptr<Geometry> CreateGeometry(const std::string& name, float heigth, float radius, int sectorCount = 32, int heightSubdivision = 10);
}