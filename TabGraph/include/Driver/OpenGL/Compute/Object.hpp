/*
* @Author: gpinchon
* @Date:   2021-05-18 15:42:07
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-18 16:45:50
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Property.hpp>
#include <Compute/Object.hpp>

#include <glm/vec3.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Compute {
class Object::Impl {
    PROPERTY(std::shared_ptr<Shader::Program>, Shader, nullptr);
    PROPERTY(std::shared_ptr<Textures::Texture>, InputTexture, nullptr);
    PROPERTY(std::shared_ptr<Textures::Texture>, OutputTexture, nullptr);
    PROPERTY(glm::ivec3, NumGroups, 0);
    PROPERTY(Object::MemoryBarrier, MemoryBarrier, Object::MemoryBarrier::Unknown);

public:
    Impl() = delete;
    Impl(const Impl&) = delete;
    Impl(Object& computeObject);
    ~Impl();
    void Run();

private:
    Object& _computeObject;
};
}

namespace OpenGL {
unsigned GetBitfield(TabGraph::Compute::Object::MemoryBarrier);
}
