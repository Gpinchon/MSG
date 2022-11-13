/*
* @Author: gpinchon
* @Date:   2021-04-10 15:35:31
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-19 10:02:08
*/

#include <Cameras/Camera.hpp>
#include <Config.hpp>
#include <Driver/OpenGL/Renderer/Light/DirectionalLightRenderer.hpp>
#include <Driver/OpenGL/Texture/Framebuffer.hpp>
#include <Light/DirectionalLight.hpp>
#include <Light/LightProbe.hpp>
#include <Nodes/Group.hpp>
#include <Renderer/FrameRenderer.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/SceneRenderer.hpp>
#include <Renderer/Shapes/GeometryRenderer.hpp>
#include <Shader/Global.hpp>
#include <Shader/Program.hpp>
#include <SphericalHarmonics.hpp>
#include <Shapes/Generators/Cube.hpp>
#include <Texture/Texture2D.hpp>
#include <Texture/Sampler.hpp>
#include <Window.hpp>

#include <GL/glew.h>
#include <array>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

using namespace TabGraph;

static inline auto DirectionnalLightGeometry()
{
    static std::weak_ptr<Shapes::Geometry> s_geometry;
    auto geometryPtr = s_geometry.lock();
    if (geometryPtr == nullptr) {
        geometryPtr = Shapes::Generators::Cube::CreateGeometry("DirectionnalLightGeometry", glm::vec3(1));
        s_geometry = geometryPtr;
    }
    return geometryPtr;
}

static inline auto DirectionnalLightVertexCode()
{
    auto lightingVertexCode =
#include "Lights/TransformGeometry.vert"
        ;
    Shader::Stage::Code shaderCode { lightingVertexCode, "TransformGeometry();" };
    return shaderCode;
}

static inline auto DirectionnalLightFragmentCode()
{
    auto deferred_frag_code =
#include "deferred.frag"
        ;
    auto randomCode =
#include "Random.glsl"
        ;
    auto shadowCode =
#include "SampleShadowMap.glsl"
        ;
    auto deferredDirLightFragmentCode =
#include "Lights/DeferredDirectionnalLight.frag"
        ;
    Shader::Stage::Code shaderCode = Shader::Stage::Code { deferred_frag_code, "FillFragmentData();" } + Shader::Stage::Code { randomCode } + Shader::Stage::Code { shadowCode } + Shader::Stage::Code { deferredDirLightFragmentCode, "Lighting();" };
    return shaderCode;
}

static inline auto DeferredDirectionnalLightShader()
{
    auto shader = std::make_shared<Shader::Program>("DirectionnalLightShader");
    shader->SetDefine("Pass", "DeferredLighting");
    shader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, DirectionnalLightVertexCode()));
    shader->Attach(Shader::Stage(Shader::Stage::Type::Fragment, DirectionnalLightFragmentCode()));
    return shader;
}

static inline auto ProbeDirectionnalLightShader()
{
    static std::weak_ptr<Shader::Program> s_shader;
    auto shaderPtr = s_shader.lock();
    if (shaderPtr == nullptr) {
        auto LayeredCubemapRenderCode =
#include "LayeredCubemapRender.geom"
            ;
        auto deferredVertexCode =
#include "deferred.vert"
            ;
        auto dirLightFragCode =
#include "Lights/ProbeDirectionnalLight.frag"
            ;
        shaderPtr = std::make_shared<Shader::Program>("DirectionnalLUTShader");
        shaderPtr->Attach(Shader::Stage(Shader::Stage::Type::Geometry, { LayeredCubemapRenderCode, "LayeredCubemapRender();" }));
        shaderPtr->Attach(Shader::Stage(Shader::Stage::Type::Vertex, { deferredVertexCode, "FillVertexData();" }));
        shaderPtr->Attach(Shader::Stage(Shader::Stage::Type::Fragment, { dirLightFragCode, "Lighting();" }));
        s_shader = shaderPtr;
    }
    return shaderPtr;
}

namespace TabGraph::Renderer {
DirectionalLightRenderer::DirectionalLightRenderer(TabGraph::Lights::DirectionalLight& light)
    : LightRenderer(light)
{
    _probeShader = ProbeDirectionnalLightShader();
    _deferredShader = DeferredDirectionnalLightShader();
    _deferredGeometry = DirectionnalLightGeometry();
}
void DirectionalLightRenderer::Render(const Renderer::Options& options)
{
    if (options.pass == Renderer::Options::Pass::ShadowDepth)
        _RenderShadow(static_cast<TabGraph::Lights::DirectionalLight&>(_light), options);
    else if (options.pass == Renderer::Options::Pass::DeferredLighting)
        _RenderDeferredLighting(static_cast<TabGraph::Lights::DirectionalLight&>(_light), options);
}

void DirectionalLightRenderer::UpdateLightProbe(const Renderer::Options& options, TabGraph::Lights::Probe& lightProbe)
{
    auto& dirLight { static_cast<TabGraph::Lights::DirectionalLight&>(_light) };
    if (_dirty) {
        _SHDiffuse = lightProbe.GetSphericalHarmonics().ProjectFunction(
            [&](const SphericalHarmonics::Sample& sample) {
                return dirLight.GetDiffuseFactor() * dirLight.GetColor() * glm::dot(sample.vec, glm::normalize(dirLight.GetDirection()));
            });
        _dirty = false;
    }
    for (auto i = 0u; i < std::min(_SHDiffuse.size(), lightProbe.GetDiffuseSH().size()); ++i)
        lightProbe.GetDiffuseSH().at(i) += _SHDiffuse.at(i);
    OpenGL::Framebuffer::Bind(lightProbe.GetReflectionBuffer());
    options.renderer->SetViewPort(lightProbe.GetReflectionBuffer()->GetSize());
    _probeShader->Use()
        .SetUniform("SpecularMode", true)
        .SetUniform("Light.SpecularFactor", dirLight.GetSpecularFactor())
        .SetUniform("Light.DiffuseFactor", dirLight.GetDiffuseFactor())
        .SetUniform("Light.Color", dirLight.GetColor())
        .SetUniform("Light.Direction", dirLight.GetDirection());
    Renderer::Render(options.renderer->GetDisplayQuad(), true);
    OpenGL::Framebuffer::Bind(nullptr);
}

void DirectionalLightRenderer::_RenderDeferredLighting(TabGraph::Lights::DirectionalLight& light, const Renderer::Options& options)
{
    auto geometryBuffer = options.renderer->DeferredGeometryBuffer();
    glm::vec3 geometryPosition;
    if (light.GetInfinite())
        geometryPosition = options.camera->GetWorldPosition();
    else
        geometryPosition = light.GetParent() ? light.GetParent()->GetWorldPosition() + light.GetLocalPosition() : light.GetLocalPosition();
    if (light.GetCastShadow()) {
        _deferredShader->SetDefine("SHADOWBLURRADIUS", std::to_string(light.GetShadowBlurRadius()));
        _deferredShader->SetDefine("SHADOW");
    } else
        _deferredShader->RemoveDefine("SHADOW");
    _deferredShader->Use()
        .SetUniform("Light.DiffuseFactor", light.GetDiffuseFactor())
        .SetUniform("Light.SpecularFactor", light.GetSpecularFactor())
        .SetUniform("Light.Color", light.GetColor())
        .SetUniform("Light.Direction", light.GetDirection())
        .SetTexture("Light.Shadow", light.GetCastShadow() ? _shadowBuffer->GetDepthBuffer() : nullptr)
        .SetUniform("Light.Max", light.GetMax())
        .SetUniform("Light.Min", light.GetMin())
        .SetUniform("Light.Projection", (light.GetInfinite() ? DirectionalLightShadowProjectionInfinite(light, options).GetMatrix() : DirectionalLightShadowProjectionFinite(light).GetMatrix()) * DirectionalLightShadowViewMatrix(light))
        .SetUniform("Light.Infinite", light.GetInfinite())
        .SetUniform("GeometryMatrix", glm::translate(geometryPosition) * light.GetLocalScaleMatrix())
        .SetTexture("Texture.Geometry.CDiff", geometryBuffer->GetColorBuffer(0))
        .SetTexture("Texture.Geometry.F0", geometryBuffer->GetColorBuffer(1))
        .SetTexture("Texture.Geometry.Normal", geometryBuffer->GetColorBuffer(2))
        .SetTexture("Texture.Geometry.Depth", geometryBuffer->GetDepthBuffer());
    glCullFace(GL_FRONT);
    Renderer::Render(_deferredGeometry);
    glCullFace(GL_BACK);
    _deferredShader->Done();
}

void DirectionalLightRenderer::_RenderShadow(TabGraph::Lights::DirectionalLight& light, const Renderer::Options& options)
{
    if (light.GetCastShadow()) {
        auto shadowRes { glm::ivec2(light.GetShadowResolution()) };
        if (_shadowBuffer == nullptr) {
            auto shadowBuffer { std::make_shared<Textures::Texture2D>(shadowRes, Pixel::SizedFormat::Depth24) };
            shadowBuffer->GetSampler()->SetCompareMode(Textures::Sampler::CompareMode::CompareRefToTexture);
            shadowBuffer->GetSampler()->SetCompareFunc(Textures::Sampler::CompareFunc::LessEqual);
            shadowBuffer->SetMipMapNbr(1);
            _shadowBuffer = std::make_shared<Framebuffer>(shadowRes);
            _shadowBuffer->SetDepthBuffer(shadowBuffer);
        }
        _shadowBuffer->SetSize(shadowRes);
        light.GetInfinite() ? _RenderShadowInfinite(light, options) : _RenderShadowFinite(light, options);
    }
}

void DirectionalLightRenderer::_RenderShadowInfinite(TabGraph::Lights::DirectionalLight& light, const Renderer::Options& options)
{
    auto radius = glm::distance(light.GetWorldPosition(), light.GetMax());
    auto camPos = light.GetWorldPosition() - light.GetDirection() * radius;
    static auto tempCamera(std::make_shared<Cameras::Camera>("dirLightCamera"));
    const auto projection { DirectionalLightShadowProjectionInfinite(light, options) };
    const auto viewMatrix { DirectionalLightShadowViewMatrix(light) };
    {
        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(viewMatrix, scale, rotation, translation, skew, perspective);
        tempCamera->SetLocalPosition(translation);
        tempCamera->SetLocalRotation(rotation);
        tempCamera->SetLocalScale(scale);
        tempCamera->SetProjection(projection);
    }

    Shader::Global::SetUniform("Camera.Position", camPos);
    Shader::Global::SetUniform("Camera.Matrix.View", viewMatrix);
    Shader::Global::SetUniform("Camera.Matrix.Projection", projection);
    OpenGL::Framebuffer::Bind(_shadowBuffer);
    options.renderer->SetViewPort(_shadowBuffer->GetSize());
    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    Renderer::Render(options.scene, { Renderer::Options::Pass::ShadowDepth, Renderer::Options::Mode::All, tempCamera, options.scene, options.renderer, options.frameNumber, options.delta });
    OpenGL::Framebuffer::Bind(nullptr);
    //options.scene->SetCurrentCamera(camera);
    Shader::Global::SetUniform("Camera.Position", options.camera->GetWorldPosition());
    Shader::Global::SetUniform("Camera.Matrix.View", options.camera->GetViewMatrix());
    Shader::Global::SetUniform("Camera.Matrix.Projection", options.camera->GetProjection());
}

void DirectionalLightRenderer::_RenderShadowFinite(TabGraph::Lights::DirectionalLight& light, const Renderer::Options& options)
{
    auto radius = glm::distance(light.GetWorldPosition(), light.GetMax());
    auto camPos = light.GetWorldPosition() - light.GetDirection() * radius;
    static auto tempCamera(std::make_shared<Cameras::Camera>("dirLightCamera"));
    const auto projection { DirectionalLightShadowProjectionFinite(light) };
    const auto viewMatrix { DirectionalLightShadowViewMatrix(light) };
    {
        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(viewMatrix, scale, rotation, translation, skew, perspective);
        tempCamera->SetLocalPosition(translation);
        tempCamera->SetLocalRotation(rotation);
        tempCamera->SetLocalScale(scale);
        tempCamera->SetProjection(projection);
    }

    Shader::Global::SetUniform("Camera.Position", camPos);
    Shader::Global::SetUniform("Camera.Matrix.View", viewMatrix);
    Shader::Global::SetUniform("Camera.Matrix.Projection", projection);
    OpenGL::Framebuffer::Bind(_shadowBuffer);
    options.renderer->SetViewPort(_shadowBuffer->GetSize());
    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    Renderer::Render(options.scene, { Renderer::Options::Pass::ShadowDepth, Renderer::Options::Mode::All, tempCamera, options.scene, options.renderer, options.frameNumber, options.delta });
    OpenGL::Framebuffer::Bind(nullptr);
    //options.scene->SetCurrentCamera(camera);
    Shader::Global::SetUniform("Camera.Position", options.camera->GetWorldPosition());
    Shader::Global::SetUniform("Camera.Matrix.View", options.camera->GetViewMatrix());
    Shader::Global::SetUniform("Camera.Matrix.Projection", options.camera->GetProjection());
}

static inline auto GetUp(const glm::vec3& direction)
{
    auto up = glm::vec3(0, 1, 0);
    if (glm::abs(glm::dot(normalize(direction), up)) > .9999f)
        up = glm::vec3(1, 0, 0);
    return up;
}

glm::mat4 DirectionalLightShadowViewMatrix(TabGraph::Lights::DirectionalLight& light)
{
    auto radius = glm::distance(light.GetWorldPosition(), light.GetMax());
    auto camPos = light.GetWorldPosition() - light.GetDirection() * radius;
    return glm::lookAt(camPos, light.GetWorldPosition(), GetUp(light.GetDirection()));
}

Cameras::Projection DirectionalLightShadowProjectionInfinite(TabGraph::Lights::DirectionalLight& light, const Renderer::Options& options)
{
    auto viewMatrix = DirectionalLightShadowViewMatrix(light);

    auto vertex = options.camera->ExtractFrustum();
    glm::vec3 maxOrtho = viewMatrix * glm::vec4(vertex.at(0), 1);
    glm::vec3 minOrtho = viewMatrix * glm::vec4(vertex.at(0), 1);
    for (auto& v : vertex) {
        v = viewMatrix * glm::vec4(v, 1);
        maxOrtho.x = std::max(maxOrtho.x, v.x);
        maxOrtho.y = std::max(maxOrtho.y, v.y);
        maxOrtho.z = std::max(maxOrtho.z, v.z);
        minOrtho.x = std::min(minOrtho.x, v.x);
        minOrtho.y = std::min(minOrtho.y, v.y);
        minOrtho.z = std::min(minOrtho.z, v.z);
    }
    auto limits = glm::vec4(minOrtho.x, maxOrtho.x, minOrtho.y, maxOrtho.y);
    auto zfar = -maxOrtho.z;
    auto znear = -minOrtho.z;
    Cameras::Projection proj = Cameras::Projection::Orthographic{
        maxOrtho.x - minOrtho.x, maxOrtho.y - minOrtho.y, znear, zfar
    };
    proj.SetTemporalJitterIntensity(0);
    return proj;
}

Cameras::Projection DirectionalLightShadowProjectionFinite(TabGraph::Lights::DirectionalLight& light)
{
    auto viewMatrix = DirectionalLightShadowViewMatrix(light);

    static std::array<glm::vec3, 7> vertex {};
    const auto min { light.GetMin() }, max { light.GetMax() };
    vertex.at(0) = max;
    vertex.at(1) = glm::vec3(min.x, min.y, max.z);
    vertex.at(2) = glm::vec3(min.x, max.y, max.z);
    vertex.at(3) = glm::vec3(min.x, max.y, min.z);
    vertex.at(4) = glm::vec3(max.x, max.y, min.z);
    vertex.at(5) = glm::vec3(max.x, min.y, min.z);
    vertex.at(6) = glm::vec3(max.x, min.y, max.z);
    glm::vec3 maxOrtho = viewMatrix * glm::vec4(min, 1);
    glm::vec3 minOrtho = viewMatrix * glm::vec4(min, 1);
    for (auto& v : vertex) {
        v = viewMatrix * glm::vec4(v, 1);
        maxOrtho.x = std::max(maxOrtho.x, v.x);
        maxOrtho.y = std::max(maxOrtho.y, v.y);
        maxOrtho.z = std::max(maxOrtho.z, v.z);
        minOrtho.x = std::min(minOrtho.x, v.x);
        minOrtho.y = std::min(minOrtho.y, v.y);
        minOrtho.z = std::min(minOrtho.z, v.z);
    }
    auto limits = glm::vec4(minOrtho.x, maxOrtho.x, minOrtho.y, maxOrtho.y);
    auto zfar = -maxOrtho.z;
    auto znear = -minOrtho.z;
    Cameras::Projection proj = Cameras::Projection::Orthographic {
        maxOrtho.x - minOrtho.x, maxOrtho.y - minOrtho.y, znear, zfar
    };
    proj.SetTemporalJitterIntensity(0);
    return proj;
}
}