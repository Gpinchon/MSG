/*
* @Author: gpinchon
* @Date:   2021-03-24 16:05:17
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-24 16:12:56
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Renderer/Shapes/ShapeRenderer.hpp>
#include <Core/Inherit.hpp>

#include <GL/glew.h>
#include <array>
#include <glm/glm.hpp>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Shapes {
class Skybox;
}
namespace Shader {
class Program;
}
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Renderer {
struct Options;
class SkyboxRenderer : public Core::Inherit<ShapeRenderer, SkyboxRenderer> {
public:
    SkyboxRenderer(Shapes::Skybox&);
    SkyboxRenderer(SkyboxRenderer&) = delete;
    void Render(const Options& options);
    virtual void OnFrameBegin(const Options& options) override;
    virtual void Render(const Options& options, const glm::mat4& parentTransform, const glm::mat4& parentLastTransform) override;
    virtual void OnFrameEnd(const Options& options) override;

private:
    std::shared_ptr<Shader::Program> _shader;
    Shapes::Skybox& _skybox;
};
};