/*
* @Author: gpinchon
* @Date:   2021-03-23 13:41:11
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-12 14:23:12
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Renderer/Shapes/ShapeRenderer.hpp>

#include <GL/glew.h>
#include <array>
#include <glm/glm.hpp>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Textures {
class TextureBuffer;
}
namespace Shapes {
class Mesh;
}
}

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Renderer {
struct Options;
class MeshRenderer : public ShapeRenderer {
public:
    MeshRenderer(Shapes::Mesh&);
    MeshRenderer(const MeshRenderer&) = delete;
    void Load();
    virtual void OnFrameBegin(const Renderer::Options& options) override;
    virtual void Render(const Options& options, const glm::mat4& parentTransform, const glm::mat4& parentLastTransform) override;
    virtual void OnFrameEnd(const Renderer::Options& options) override;

private:
    std::array<GLsync, 2> _drawSync { nullptr };
    std::array<std::shared_ptr<Textures::TextureBuffer>, 2> _jointMatrices { nullptr };
    glm::mat4 _prevTransformMatrix { 1 };
    int _jointMatricesIndex { 0 };
    bool _loaded { false };
    Shapes::Mesh& _mesh;
};
}
