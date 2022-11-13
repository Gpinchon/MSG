/*
* @Author: gpinchon
* @Date:   2020-10-01 15:30:47
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-19 00:12:43
*/

#include <Compute/Object.hpp>

#if RENDERINGAPI == OpenGL
#include <Driver/OpenGL/Compute/Object.hpp>
#endif

namespace TabGraph::Compute {
Object::Object(std::shared_ptr<Shader::Program> computeShader)
    : _impl(new Object::Impl(*this))
{
    SetProgram(computeShader);
}

Object::~Object()
{
}

std::shared_ptr<Shader::Program> Object::GetProgram() const
{
    return _impl->GetShader();
}

std::shared_ptr<Textures::Texture> Object::GetInputTexture() const
{
    return _impl->GetInputTexture();
}

std::shared_ptr<Textures::Texture> Object::GetOutputTexture() const
{
    return _impl->GetOutputTexture();
}

glm::ivec3 Object::GetNumGroups() const
{
    return _impl->GetNumGroups();
}

Object::MemoryBarrier Object::GetMemoryBarrier() const
{
    return _impl->GetMemoryBarrier();
}

void Object::SetProgram(std::shared_ptr<Shader::Program> shader)
{
    _impl->SetShader(shader);
}

void Object::SetInputTexture(std::shared_ptr<Textures::Texture> texture)
{
    _impl->SetInputTexture(texture);
}

void Object::SetOutputTexture(std::shared_ptr<Textures::Texture> texture)
{
    _impl->SetOutputTexture(texture);
}

void Object::SetNumGroups(glm::ivec3 numGroups)
{
    _impl->SetNumGroups(numGroups);
}

void Object::SetMemoryBarrier(MemoryBarrier memoryBarrier)
{
    _impl->SetMemoryBarrier(memoryBarrier);
}

void Object::Run()
{
    _impl->Run();
}
}
