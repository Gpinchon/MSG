/*
* @Author: gpinchon
* @Date:   2021-04-11 14:44:44
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-19 10:02:30
*/

#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>
#include <Assets/Image.hpp>
#include <Cameras/Camera.hpp>
#include <Driver/OpenGL/Renderer/Light/HDRLightRenderer.hpp>
#include <Driver/OpenGL/Texture/Framebuffer.hpp>
#include <Light/HDRLight.hpp>
#include <Light/LightProbe.hpp>
#include <Renderer/FrameRenderer.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/Shapes/GeometryRenderer.hpp>
#include <Shader/Program.hpp>
#include <Shader/Stage.hpp>
#include <SphericalHarmonics.hpp>
#include <Shapes/Generators/Cube.hpp>
#include <Shapes/Geometry.hpp>
#include <Texture/Texture2D.hpp>
#include <Texture/TextureCubemap.hpp>
#include <Nodes/Group.hpp>

#include <GL/glew.h>
#include <glm/gtx/transform.hpp>

namespace TabGraph::Renderer {
const glm::vec2 invAtan = glm::vec2(0.1591, 0.3183);
static inline glm::vec2 SampleSphericalMap(glm::vec3 xyz)
{
    glm::vec2 uv = glm::vec2(atan2(xyz.z, xyz.x), asin(xyz.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

static inline auto HDRLightGeometry()
{
    static std::weak_ptr<Shapes::Geometry> s_geometry;
    auto geometryPtr = s_geometry.lock();
    if (geometryPtr == nullptr) {
        geometryPtr = Shapes::Generators::Cube::CreateGeometry("HDRLightGeometry", glm::vec3(1));
        s_geometry = geometryPtr;
    }
    return geometryPtr;
}

static inline auto DeferredHDRLightVertexCode()
{
    auto lightingVertexCode =
#include "Lights/TransformGeometry.vert"
        ;
    Shader::Stage::Code shaderCode { lightingVertexCode, "TransformGeometry();" };
    return shaderCode;
}

static inline auto DeferredHDRLightFragmentCode()
{
    auto deferred_frag_code =
#include "deferred.frag"
        ;
    auto randomCode =
#include "Random.glsl"
        ;
    auto SHCode =
#include "sphericalHarmonics.glsl"
        ;
    auto lightingFragmentCode =
#include "Lights/DeferredHDRLight.frag"
        ;
    Shader::Stage::Code shaderCode = Shader::Stage::Code { deferred_frag_code, "FillFragmentData();" } + Shader::Stage::Code { SHCode } + Shader::Stage::Code { lightingFragmentCode, "Lighting();" };
    return shaderCode;
}

static SphericalHarmonics s_SH { 50 };
HDRLightRenderer::HDRLightRenderer(TabGraph::Lights::HDRLight& light)
    : LightRenderer(light)
{
    auto LayeredCubemapRenderCode =
#include "LayeredCubemapRender.geom"
        ;
    auto deferredVertexCode =
#include "deferred.vert"
        ;
    auto dirLightFragCode =
#include "Lights/ProbeHDRLight.frag"
        ;
    _probeShader = std::make_shared<Shader::Program>("ProbeHDRLightShader");
    _probeShader->Attach(Shader::Stage(Shader::Stage::Type::Geometry, { LayeredCubemapRenderCode, "LayeredCubemapRender();" }));
    _probeShader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, { deferredVertexCode, "FillVertexData();" }));
    _probeShader->Attach(Shader::Stage(Shader::Stage::Type::Fragment, { dirLightFragCode, "Lighting();" }));
    _deferredShader = std::make_shared<Shader::Program>("DeferredHDRLightShader");
    _deferredShader->SetDefine("Pass", "DeferredLighting");
    _deferredShader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, DeferredHDRLightVertexCode()));
    _deferredShader->Attach(Shader::Stage(Shader::Stage::Type::Fragment, DeferredHDRLightFragmentCode()));
    _deferredGeometry = HDRLightGeometry();
}

void HDRLightRenderer::Render(const Renderer::Options& options)
{
    if (options.pass == Renderer::Options::Pass::DeferredLighting) {
        _RenderDeferredLighting(static_cast<TabGraph::Lights::HDRLight&>(_light), options);
    }
}

void HDRLightRenderer::UpdateLightProbe(const Renderer::Options& options, TabGraph::Lights::Probe& lightProbe)
{
    auto& hdrLight { static_cast<TabGraph::Lights::HDRLight&>(_light) };
    _Update(hdrLight);
    for (auto i = 0u; i < std::min(_SHDiffuse.size(), lightProbe.GetDiffuseSH().size()); ++i)
        lightProbe.GetDiffuseSH().at(i) += _SHDiffuse.at(i);
    OpenGL::Framebuffer::Bind(lightProbe.GetReflectionBuffer());
    options.renderer->SetViewPort(lightProbe.GetReflectionBuffer()->GetSize());
    _probeShader->Use()
        .SetUniform("Light.DiffuseFactor", hdrLight.GetDiffuseFactor())
        .SetUniform("Light.SpecularFactor", hdrLight.GetSpecularFactor())
        .SetUniform("Light.Min", hdrLight.GetMin())
        .SetUniform("Light.Max", hdrLight.GetMax())
        .SetUniform("Light.Infinite", hdrLight.GetInfinite())
        .SetUniform("ProbePosition", lightProbe.GetAbsolutePosition())
        .SetTexture("ReflectionMap", hdrLight.GetHDRTexture());
    Renderer::Render(options.renderer->GetDisplayQuad(), true);
    OpenGL::Framebuffer::Bind(nullptr);
}

void HDRLightRenderer::_RenderDeferredLighting(TabGraph::Lights::HDRLight& light, const Renderer::Options& options)
{
    _Update(light);
    auto geometryBuffer = options.renderer->DeferredGeometryBuffer();
    glm::vec3 geometryPosition;
    if (light.GetInfinite())
        geometryPosition = options.camera->GetWorldPosition();
    else
        geometryPosition = light.GetWorldPosition();//light.GetParent() ? light.GetParent()->GetWorldPosition() + light.GetLocalPosition() : light.GetLocalPosition();
    _deferredShader->Use()
        .SetUniform("Light.DiffuseFactor", light.GetDiffuseFactor())
        .SetUniform("Light.SpecularFactor", light.GetSpecularFactor())
        .SetUniform("Light.Max", light.GetMax())
        .SetUniform("Light.Min", light.GetMin())
        .SetUniform("Light.Infinite", light.GetInfinite())
        .SetTexture("ReflectionMap", light.GetHDRTexture())
        .SetTexture("DefaultBRDFLUT", options.renderer->GetDefaultBRDFLUT())
        .SetUniform("SH[0]", _SHDiffuse.data(), _SHDiffuse.size())
        .SetUniform("GeometryMatrix", glm::translate(geometryPosition) * light.GetLocalScaleMatrix())
        .SetTexture("Texture.Geometry.CDiff", geometryBuffer->GetColorBuffer(0))
        .SetTexture("Texture.Geometry.F0", geometryBuffer->GetColorBuffer(1))
        .SetTexture("Texture.Geometry.Normal", geometryBuffer->GetColorBuffer(2))
        .SetTexture("Texture.Geometry.Depth", geometryBuffer->GetDepthBuffer());
    glCullFace(GL_FRONT);
    Renderer::Render(_deferredGeometry, false);
    glCullFace(GL_BACK);
    _deferredShader->Done();
}

void HDRLightRenderer::_Update(TabGraph::Lights::HDRLight& light)
{
    if (!_dirty)
        return;
    auto asset { light.GetHDRImage() };
    Assets::Parser::AddParsingTask({ Assets::Parser::ParsingTask::Type::Sync, asset });
    auto images = asset->Get<Assets::Image>();
    assert(!images.empty());
    auto image { images.at(0) };
    _SHDiffuse = s_SH.ProjectFunction(
        [&](const SphericalHarmonics::Sample& sample) {
            auto dir { -sample.vec };
            auto uv { SampleSphericalMap(dir) };
            glm::ivec2 texCoord {
                uv.x * image->GetSize().x,
                uv.y * image->GetSize().y
            };
            texCoord = glm::clamp(texCoord, glm::ivec2(0), image->GetSize() - 1);
            glm::vec3 color { Pixel::LinearToSRGB(image->GetColor(texCoord)) };
            return glm::clamp(color, glm::vec3(0), glm::vec3(1));
        });
    light.SetHDRImage(nullptr);
    _dirty = false;
}
}
