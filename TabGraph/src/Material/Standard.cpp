/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:02:24
*/

#include <Material/Standard.hpp>
#include <Material/Extension.hpp>
#include <Assets/Asset.hpp>
#include <Shader/Program.hpp>
#include <Shader/Stage.hpp>
#include <Texture/Texture2D.hpp>

using namespace TabGraph;

static inline auto GetSHExtension()
{
    auto shaderCode =
#include "sphericalHarmonics.glsl"
        ;
    return Shader::Stage::Code(shaderCode);
}

static inline auto GetCheckOpacityExtension()
{
    auto checkOpacity =
#include "checkOpacity.glsl"
        ;
    return Shader::Stage::Code(checkOpacity, "CheckOpacity();");
}

static inline auto GetForwardLitPassExtension()
{
    auto forwardLitPass =
#include "forwardLit.frag"
        ;
    return Shader::Stage::Code(forwardLitPass, "ComputeColor();");
}

static inline auto GetForwardVertexExtension()
{
    auto forward_vert_code =
#include "forward.vert"
        ;
    return Shader::Stage::Code(forward_vert_code, "FillVertexData();");
}

static inline auto GetForwardFragmentExtension()
{
    auto forward_frag_code =
#include "forward.frag"
        ;
    return Shader::Stage::Code(forward_frag_code, "FillFragmentData();");
}

namespace TabGraph::Material {
Standard::Standard(const std::string& name)
    : Inherit(name)
{
    SetShader(Renderer::Options::Pass::DeferredGeometry, std::make_shared<Shader::Program>(GetName() + "_geometryShader"));
    GetShader(Renderer::Options::Pass::DeferredGeometry)->SetDefine("Pass", "DeferredGeometry");
    GetShader(Renderer::Options::Pass::DeferredGeometry)->Attach(Shader::Stage(Shader::Stage::Type::Vertex, GetForwardVertexExtension()));
    GetShader(Renderer::Options::Pass::DeferredGeometry)->Attach(Shader::Stage(Shader::Stage::Type::Fragment, GetForwardFragmentExtension() + GetCheckOpacityExtension()));

    SetShader(Renderer::Options::Pass::ForwardTransparent, std::make_shared<Shader::Program>(GetName() + "_materialShader"));
    GetShader(Renderer::Options::Pass::ForwardTransparent)->SetDefine("Pass", "ForwardTransparent");
    GetShader(Renderer::Options::Pass::ForwardTransparent)->Attach(Shader::Stage(Shader::Stage::Type::Vertex, GetSHExtension() + GetForwardVertexExtension()));
    GetShader(Renderer::Options::Pass::ForwardTransparent)->Attach(Shader::Stage(Shader::Stage::Type::Fragment, GetForwardFragmentExtension() + GetForwardLitPassExtension()));

    SetShader(Renderer::Options::Pass::ShadowDepth, std::make_shared<Shader::Program>(GetName() + "_depthShader"));
    GetShader(Renderer::Options::Pass::ShadowDepth)->SetDefine("Pass", "ShadowDepth");
    GetShader(Renderer::Options::Pass::ShadowDepth)->Attach(Shader::Stage(Shader::Stage::Type::Vertex, GetForwardVertexExtension()));
    GetShader(Renderer::Options::Pass::ShadowDepth)->Attach(Shader::Stage(Shader::Stage::Type::Fragment, GetForwardFragmentExtension() + GetCheckOpacityExtension()));

    SetOpacity(1);
}

void Standard::AddExtension(std::shared_ptr<Extension> extension)
{
    _extensions.insert(extension);
    GetShader(Renderer::Options::Pass::DeferredGeometry)->GetStage(Shader::Stage::Type::Fragment) -= GetCheckOpacityExtension();
    GetShader(Renderer::Options::Pass::DeferredGeometry)->GetStage(Shader::Stage::Type::Fragment) += extension->GetCode();
    GetShader(Renderer::Options::Pass::DeferredGeometry)->GetStage(Shader::Stage::Type::Fragment) += GetCheckOpacityExtension();

    GetShader(Renderer::Options::Pass::ForwardTransparent)->GetStage(Shader::Stage::Type::Fragment) -= GetForwardLitPassExtension();
    GetShader(Renderer::Options::Pass::ForwardTransparent)->GetStage(Shader::Stage::Type::Fragment) += extension->GetCode();
    GetShader(Renderer::Options::Pass::ForwardTransparent)->GetStage(Shader::Stage::Type::Fragment) += GetForwardLitPassExtension();

    GetShader(Renderer::Options::Pass::ShadowDepth)->GetStage(Shader::Stage::Type::Fragment) -= GetCheckOpacityExtension();
    GetShader(Renderer::Options::Pass::ShadowDepth)->GetStage(Shader::Stage::Type::Fragment) += extension->GetCode();
    GetShader(Renderer::Options::Pass::ShadowDepth)->GetStage(Shader::Stage::Type::Fragment) += GetCheckOpacityExtension();
}

void Standard::RemoveExtension(std::shared_ptr<Extension> extension)
{
    GetShader(Renderer::Options::Pass::DeferredGeometry)->GetStage(Shader::Stage::Type::Fragment) -= extension->GetCode();
    GetShader(Renderer::Options::Pass::ForwardTransparent)->GetStage(Shader::Stage::Type::Fragment) -= extension->GetCode();
    GetShader(Renderer::Options::Pass::ShadowDepth)->GetStage(Shader::Stage::Type::Fragment) -= extension->GetCode();
    _extensions.erase(extension);
}

std::shared_ptr<Extensions::Extension> Standard::GetExtension(const std::string& name) const
{
    for (const auto& extension : _extensions)
        if (extension->GetName() == name) return extension;
    return nullptr;
}

void Standard::Bind(const Renderer::Options::Pass& pass)
{
    switch (GetOpacityMode()) {
    case (OpacityMode::Opaque):
        GetShader(pass)->SetDefine("OpacityMode", "Opaque");
        break;
    case (OpacityMode::Mask):
        GetShader(pass)->SetDefine("OpacityMode", "Mask");
        break;
    case (OpacityMode::Blend):
        GetShader(pass)->SetDefine("OpacityMode", "Blend");
        break;
    }
    for (const auto& define : GetDefines())
        GetShader(pass)->SetDefine(define.first, define.second);
    for (const auto& extension : _extensions) {
        for (const auto& define : extension->GetDefines()) {
            GetShader(pass)->SetDefine(define.first, define.second);
        }
    }
    GetShader(pass)->Use();
    auto UVTranslation{ glm::mat3(1,0,0, 0,1,0, GetUVOffset().x, GetUVOffset().y, 1) };
    auto UVRotation{ glm::mat3(
        cos(GetUVRotation()), sin(GetUVRotation()), 0,
        -sin(GetUVRotation()), cos(GetUVRotation()), 0,
        0, 0, 1
    ) };
    auto UVScale{ glm::mat3(GetUVScale().x, 0, 0, 0, GetUVScale().y, 0, 0, 0, 1) };
    GetShader(pass)->SetUniform("UVTransform", UVTranslation * UVRotation * UVScale);
    for (const auto& color : GetColors()) {
        GetShader(pass)->SetUniform(color.first, color.second);
    }
    for (const auto& value : GetValues()) {
        GetShader(pass)->SetUniform(value.first, value.second);
    }
    for (const auto& texture : GetTextures()) {
        GetShader(pass)->SetTexture(texture.first, texture.second);
    }
    for (const auto& extension : _extensions) {
        for (const auto& color : extension->GetColors()) {
            GetShader(pass)->SetUniform(color.first, color.second);
        }
        for (const auto& value : extension->GetValues()) {
            GetShader(pass)->SetUniform(value.first, value.second);
        }
        for (const auto& texture : extension->GetTextures()) {
            GetShader(pass)->SetTexture(texture.first, texture.second);
        }
    }
    GetShader(pass)->Done();
}

void Standard::SetTextureDiffuse(std::shared_ptr<Textures::Texture2D> value)
{
    SetTexture("StandardTextureDiffuse", value);
    for (auto& shader : _shaders) {
        if (shader == nullptr)
            continue;
        value ? shader->SetDefine("TEXTURE_USE_DIFFUSE") : shader->RemoveDefine("TEXTURE_USE_DIFFUSE");
    }
}

void Standard::SetTextureEmissive(std::shared_ptr<Textures::Texture2D> value)
{
    SetTexture("StandardTextureEmissive", value);
    for (auto& shader : _shaders) {
        if (shader == nullptr)
            continue;
        value ? shader->SetDefine("TEXTURE_USE_EMISSIVE") : shader->RemoveDefine("TEXTURE_USE_EMISSIVE");
    }
}

void Standard::SetTextureNormal(std::shared_ptr<Textures::Texture2D> value)
{
    SetTexture("StandardTextureNormal", value);
    for (auto& shader : _shaders) {
        if (shader == nullptr)
            continue;
        value ? shader->SetDefine("TEXTURE_USE_NORMAL") : shader->RemoveDefine("TEXTURE_USE_NORMAL");
    }
}

void Standard::SetTextureHeight(std::shared_ptr<Textures::Texture2D> value)
{
    SetTexture("StandardTextureHeight", value);
    for (auto& shader : _shaders) {
        if (shader == nullptr)
            continue;
        value ? shader->SetDefine("TEXTURE_USE_HEIGHT") : shader->RemoveDefine("TEXTURE_USE_HEIGHT");
    }
}

void Standard::SetTextureAO(std::shared_ptr<Textures::Texture2D> value)
{
    SetTexture("StandardTextureAO", value);
    for (auto& shader : _shaders) {
        if (shader == nullptr)
            continue;
        value ? shader->SetDefine("TEXTURE_USE_AO") : shader->RemoveDefine("TEXTURE_USE_AO");
    }
}

void Standard::SetTextureBRDFLUT(std::shared_ptr<Textures::Texture2D> value) {
    SetTexture("StandardTextureBRDFLUT", value);
}
}
