/*
* @Author: gpinchon
* @Date:   2021-05-04 22:26:27
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 22:32:24
*/
#pragma once
namespace TabGraph::Pixel {
enum class SizedFormat;
enum class UnsizedFormat;
enum class Type;
}

namespace OpenGL {
unsigned GetEnum(const TabGraph::Pixel::SizedFormat&);
unsigned GetEnum(const TabGraph::Pixel::UnsizedFormat&);
unsigned GetEnum(const TabGraph::Pixel::Type&);
}