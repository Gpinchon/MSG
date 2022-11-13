/*
* @Author: gpinchon
* @Date:   2021-04-11 16:24:38
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-19 10:02:54
*/

#include <Cameras/Camera.hpp>
#include <Driver/OpenGL/Renderer/Light/SkyLightRenderer.hpp>
#include <Driver/OpenGL/Renderer/Light/DirectionalLightRenderer.hpp>
#include <Driver/OpenGL/Texture/Framebuffer.hpp>
#include <Light/SkyLight.hpp>
#include <Nodes/Group.hpp>
#include <Renderer/FrameRenderer.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/Shapes/GeometryRenderer.hpp>
#include <Shader/Program.hpp>
#include <SphericalHarmonics.hpp>
#include <Shapes/Geometry.hpp>
#include <Shapes/Generators/Cube.hpp>
#include <Texture/Texture2D.hpp>
#include <Texture/TextureCubemap.hpp>
#include <Texture/Sampler.hpp>

#include <GL/glew.h>
#include <glm/gtx/transform.hpp>

using namespace TabGraph;

static inline auto DeferredSkyLightVertexCode()
{
    auto lightingVertexCode =
#include "Lights/TransformGeometry.vert"
        ;
    static Shader::Stage::Code shaderCode { lightingVertexCode, "TransformGeometry();" };
    return shaderCode;
}

static inline auto DeferredSkyLightFragmentCode()
{
    auto deferred_frag_code =
#include "deferred.frag"
        ;
    auto SHCode =
#include "sphericalHarmonics.glsl"
        ;
    auto lightingFragmentCode =
#include "Lights/DeferredSkyLight.frag"
        ;
    Shader::Stage::Code shaderCode = Shader::Stage::Code { deferred_frag_code, "FillFragmentData();" } + Shader::Stage::Code { SHCode } + Shader::Stage::Code { lightingFragmentCode, "Lighting();" };
    return shaderCode;
}

auto SkyLightLUTShader()
{
    auto LayeredCubemapRenderCode =
#include "LayeredCubemapRender.geom"
        ;
    auto deferredVertexCode =
#include "deferred.vert"
        ;
    auto skyLightFragCode =
#include "Lights/ProbeSkyLight.frag"
        ;
    auto shader = std::make_shared<Shader::Program>("DirectionnalLUTShader");
    shader->Attach(Shader::Stage(Shader::Stage::Type::Geometry, { LayeredCubemapRenderCode, "LayeredCubemapRender();" }));
    shader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, { deferredVertexCode, "FillVertexData();" }));
    shader->Attach(Shader::Stage(Shader::Stage::Type::Fragment, { skyLightFragCode, "ComputeSkyLight();" }));
    return shader;
}

static inline auto SkyLightGeometry()
{
    static std::weak_ptr<Shapes::Geometry> s_geometry;
    auto geometryPtr = s_geometry.lock();
    if (geometryPtr == nullptr) {
        geometryPtr = Shapes::Generators::Cube::CreateGeometry("SkyLightGeometry", glm::vec3(1));
        s_geometry = geometryPtr;
    }
    return geometryPtr;
}

namespace TabGraph::Renderer {
static SphericalHarmonics s_SH { 50 };
SkyLightRenderer::SkyLightRenderer(TabGraph::Lights::SkyLight& light)
    : LightRenderer(light)
    , _diffuseLUTBuffer(std::make_shared<Framebuffer>(glm::ivec2(256)))
    , _deferredShader(std::make_shared<Shader::Program>("SkyLightShader"))
{
    _deferredShader->SetDefine("Pass", "DeferredLighting");
    _deferredShader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, DeferredSkyLightVertexCode()));
    _deferredShader->Attach(Shader::Stage(Shader::Stage::Type::Fragment, DeferredSkyLightFragmentCode()));
    _deferredGeometry = SkyLightGeometry();
}

void SkyLightRenderer::Render(const Renderer::Options& options)
{
    if (options.pass == Renderer::Options::Pass::DeferredLighting)
        _RenderDeferredLighting(static_cast<TabGraph::Lights::SkyLight&>(_light), options);
    else if (options.pass == Renderer::Options::Pass::ShadowDepth)
        _RenderDeferredLighting(static_cast<TabGraph::Lights::SkyLight&>(_light), options);
}

void SkyLightRenderer::UpdateLightProbe(const Renderer::Options& options, TabGraph::Lights::Probe& lightProbe)
{
    _UpdateLUT(static_cast<TabGraph::Lights::SkyLight&>(_light), options);
    //TODO really implement this
}

void SkyLightRenderer::_RenderDeferredLighting(TabGraph::Lights::SkyLight& light, const Renderer::Options& options)
{
    if (_dirty) {
        _UpdateLUT(light, options);
        //re-bind lighting buffer
        OpenGL::Framebuffer::Bind(options.renderer->DeferredLightingBuffer());
        options.renderer->SetViewPort(options.renderer->DeferredLightingBuffer()->GetSize());
    }
    auto geometryBuffer = options.renderer->DeferredGeometryBuffer();
    glm::vec3 geometryPosition;
    if (light.GetInfinite())
        geometryPosition = options.camera->GetWorldPosition();
    else
        geometryPosition = light.GetParent() ? light.GetParent()->GetWorldPosition() + light.GetLocalPosition() : light.GetWorldPosition();
    if (light.GetCastShadow())
        _deferredShader->SetDefine("SHADOW");
    else
        _deferredShader->RemoveDefine("SHADOW");
    _deferredShader->Use()
        .SetTexture("Light.Shadow", light.GetCastShadow() ? _shadowBuffer->GetDepthBuffer() : nullptr)
        .SetUniform("Light.SpecularFactor", light.GetSpecularFactor())
        .SetUniform("Light.DiffuseFactor", light.GetDiffuseFactor())
        .SetUniform("Light.Max", light.GetMax())
        .SetUniform("Light.Min", light.GetMin())
        .SetUniform("Light.Projection", (light.GetInfinite() ? DirectionalLightShadowProjectionInfinite(light, options) : DirectionalLightShadowProjectionFinite(light)) * DirectionalLightShadowViewMatrix(light))
        .SetUniform("Light.Infinite", light.GetInfinite())
        .SetTexture("SpecularLUT", _reflectionLUT)
        .SetTexture("DefaultBRDFLUT", options.renderer->GetDefaultBRDFLUT())
        .SetUniform("SH[0]", _SHDiffuse.data(), _SHDiffuse.size())
        .SetUniform("GeometryMatrix", glm::translate(geometryPosition) * light.GetLocalScaleMatrix())
        .SetTexture("Texture.Geometry.CDiff", geometryBuffer->GetColorBuffer(0))
        .SetTexture("Texture.Geometry.F0", geometryBuffer->GetColorBuffer(1))
        .SetTexture("Texture.Geometry.Normal", geometryBuffer->GetColorBuffer(2))
        .SetTexture("Texture.Geometry.Depth", geometryBuffer->GetDepthBuffer());
    glCullFace(GL_FRONT);
    Renderer::Render(_deferredGeometry, true);
    _deferredShader->Done();
}
void SkyLightRenderer::_UpdateLUT(TabGraph::Lights::SkyLight& light, const Renderer::Options& options)
{
    if (!_dirty)
        return;
    _SHDiffuse = s_SH.ProjectFunction(
        [&](const SphericalHarmonics::Sample& sample) {
            return min(light.GetIncidentLight(sample.vec), 1.f);
        });
    if (_reflectionLUT == nullptr) {
        _reflectionLUT = std::make_shared<Textures::TextureCubemap>(glm::ivec2(256), Pixel::SizedFormat::Float16_RGB);
    }
    static auto s_diffuseLUTBuffer = std::make_shared<Framebuffer>(glm::ivec2(256));
    s_diffuseLUTBuffer->SetColorBuffer(_reflectionLUT, 0);
    OpenGL::Framebuffer::Bind(s_diffuseLUTBuffer);
    options.renderer->SetViewPort(s_diffuseLUTBuffer->GetSize());
    SkyLightLUTShader()->Use().SetUniform("SpecularFactor", light.GetSpecularFactor()).SetUniform("DiffuseFactor", light.GetDiffuseFactor()).SetUniform("BetaMie", light.GetBetaMie()).SetUniform("BetaRayleigh", light.GetBetaRayleigh()).SetUniform("SunDirection", light.GetSunDirection()).SetUniform("SunPower", light.GetSunPower()).SetUniform("PlanetRadius", light.GetPlanetRadius()).SetUniform("AtmosphereRadius", light.GetAtmosphereRadius()).SetUniform("HRayLeigh", light.GetHRayleigh()).SetUniform("HMie", light.GetHMie());
    glClear(GL_COLOR_BUFFER_BIT);
    Renderer::Render(options.renderer->GetDisplayQuad(), true);
    OpenGL::Framebuffer::Bind(nullptr);
    s_diffuseLUTBuffer->SetColorBuffer(nullptr, 0);
    _reflectionLUT->GenerateMipmap();
    _reflectionLUT->GetSampler()->SetMinFilter(Textures::Sampler::Filter::LinearMipmapLinear);
    _dirty = false;
}
};