/*
* @Author: gpinchon
* @Date:   2021-03-24 16:07:19
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-19 10:03:25
*/

#include <Driver/OpenGL/Renderer/Shapes/SkyboxRenderer.hpp>
#include <Renderer/FrameRenderer.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/Shapes/GeometryRenderer.hpp>
#include <Shader/Program.hpp>
#include <Shapes/Skybox.hpp>
#include <Texture/TextureCubemap.hpp>
#include <Shader/Program.hpp>
#include <Shader/Stage.hpp>

namespace TabGraph::Renderer {
static inline auto EnvShader()
{
    auto deferredVertCode =
#include "deferred.vert"
        ;
    auto skyboxFragCode =
#include "Skybox.frag"
        ;
    auto shader = std::make_shared<Shader::Program>("EnvShader");
    shader->SetDefine("Pass", "DeferredLighting");
    shader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, { deferredVertCode, "FillVertexData();" }));
    shader->Attach(Shader::Stage(Shader::Stage::Type::Fragment, { skyboxFragCode, "OutputEnv();" }));
    return shader;
}

SkyboxRenderer::SkyboxRenderer(Shapes::Skybox& skybox)
    : Inherit()
    , _skybox(skybox)
{
    _shader = EnvShader();
}

void SkyboxRenderer::Render(const Options& options)
{
    _shader->Use().SetTexture("Skybox", _skybox.GetTexture());
    Renderer::Render(options.renderer->GetDisplayQuad(), true);
    _shader->Done();
}

void SkyboxRenderer::OnFrameBegin(const Options&)
{
}

void SkyboxRenderer::Render(const Options& options, const glm::mat4& parentTransform, const glm::mat4& parentLastTransform)
{
    Render(options);
}

void SkyboxRenderer::OnFrameEnd(const Options&)
{
}
};