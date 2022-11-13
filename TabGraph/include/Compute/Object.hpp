/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-18 16:06:48
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <glm/fwd.hpp>
#include <memory>
#include <string>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Textures {
class Texture;
}
namespace Shader {
class Program;
}
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Compute {
class Object {
public:
    enum class MemoryBarrier {
        Unknown = -1,
        MaxValue
    };
    class Impl;
    Object(std::shared_ptr<Shader::Program> computeShader = nullptr);
    ~Object();
    std::shared_ptr<Shader::Program> GetProgram() const;
    std::shared_ptr<Textures::Texture> GetInputTexture() const;
    std::shared_ptr<Textures::Texture> GetOutputTexture() const;
    glm::ivec3 GetNumGroups() const;
    MemoryBarrier GetMemoryBarrier() const;
    void SetProgram(std::shared_ptr<Shader::Program>);
    void SetInputTexture(std::shared_ptr<Textures::Texture>);
    void SetOutputTexture(std::shared_ptr<Textures::Texture>);
    void SetNumGroups(glm::ivec3);
    void SetMemoryBarrier(MemoryBarrier);
    void Run();

protected:
    std::unique_ptr<Impl> _impl;
};
}
