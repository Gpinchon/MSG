/*
* @Author: gpinchon
* @Date:   2021-03-22 20:42:01
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-12 16:32:30
*/

#pragma once

#include <memory>

class Geometry;

namespace Renderer {
struct Options;
void Render(std::shared_ptr<Geometry> geometry, bool doubleSided = false);
};