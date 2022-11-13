/*
* @Author: gpinchon
* @Date:   2021-03-23 13:40:55
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-19 10:11:27
*/

#include <Config.hpp>
#include <Driver/OpenGL/Renderer/Shapes/MeshRenderer.hpp>
#include <Driver/OpenGL/Renderer/SceneRenderer.hpp>
#include <Light/LightProbe.hpp>
#include <Material/Standard.hpp>
#include <Renderer/FrameRenderer.hpp>
#include <Renderer/Framebuffer.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/Shapes/GeometryRenderer.hpp>
#include <Nodes/Scene.hpp>
#include <Shader/Program.hpp>
#include <Shapes/Geometry.hpp>
#include <Shapes/Mesh/Mesh.hpp>
#include <Shapes/Mesh/MeshSkin.hpp>
#include <Texture/Texture2D.hpp>
#include <Texture/TextureBuffer.hpp>
#include <Nodes/Node.hpp>
#include <Debug.hpp>
#include <glm/gtc/matrix_inverse.hpp>

namespace TabGraph::Renderer {
MeshRenderer::MeshRenderer(Shapes::Mesh& mesh)
    : _mesh(mesh)
{
}

void MeshRenderer::Load()
{
    if (_loaded)
        return;
    if (_mesh.GetSkin()) {
        debugLog(_mesh.GetName() + " : Create Skin");
        for (auto i = 0; i < _jointMatrices.size(); ++i) {
            if (_jointMatrices.at(i) != nullptr)
                continue;
            auto bufferAccessor = Buffer::TypedAccessor<glm::mat4>(_mesh.GetSkin()->Joints().size());
            bufferAccessor.GetBufferView()->SetType(Buffer::View::Type::TextureBuffer);
            bufferAccessor.GetBufferView()->SetMode(Buffer::View::Mode::Persistent);
            bufferAccessor.GetBufferView()->SetPersistentMappingMode(Buffer::View::MappingMode::WriteOnly);
            _jointMatrices.at(i) = std::make_shared<Textures::TextureBuffer>(Pixel::SizedFormat::Float32_RGBA, bufferAccessor.GetBufferView());
            _jointMatrices.at(i)->Load();
        }
    }
    _loaded = true;
}

void MeshRenderer::OnFrameBegin(const Renderer::Options& options)
{
    _mesh.Load();
    Load();
    _jointMatricesIndex = (_jointMatricesIndex + 1) % _jointMatrices.size();
    auto meshSkin { _mesh.GetSkin() };
    if (meshSkin == nullptr)
        return;
    if (glIsSync(_drawSync.at(_jointMatricesIndex))) {
        while (glClientWaitSync(_drawSync.at(_jointMatricesIndex), 0, 1) == GL_TIMEOUT_EXPIRED) { }
        glDeleteSync(_drawSync.at(_jointMatricesIndex));
        _drawSync.at(_jointMatricesIndex) = nullptr;
    }
    const auto &joints = meshSkin->Joints();
    Buffer::TypedAccessor<glm::mat4> jointMatricesAccessor(_jointMatrices.at(_jointMatricesIndex)->GetBuffer(), 0, joints.size());
    auto& inverseBindMatrices { meshSkin->GetInverseBindMatrices() };
    for (auto index = 0u; index < joints.size(); ++index) {
        const auto jointMatrixIndex { index };
        auto jointMatrix = joints.at(index)->GetWorldTransformMatrix() * inverseBindMatrices.at(index);
        jointMatricesAccessor.at(jointMatrixIndex) = jointMatrix;
    }
}

void MeshRenderer::Render(
    const Options& options,
    const glm::mat4& parentTransform,
    const glm::mat4& parentLastTransform)
{
    auto transformMatrix = parentTransform * _mesh.GetGeometryTransform();
    auto prevTransform = parentLastTransform * _prevTransformMatrix;
    auto skinTransform = glm::inverse(parentTransform);
    auto skinPrevTransform = glm::inverse(parentLastTransform);
    auto& lightProbe = Renderer::GetRenderer(options.scene).GetClosestLightProbe(transformMatrix * glm::vec4(0, 0, 0, 1));

    auto normal_matrix = glm::inverseTranspose(glm::mat3(transformMatrix));

    std::shared_ptr<Shader::Program> last_shader;
    auto skinned { _mesh.GetSkin() != nullptr };
    auto jointsIndex = _jointMatricesIndex;
    auto prevJointsIndex = (_jointMatricesIndex - 1) % _jointMatrices.size();
    auto fastTransparency { Config::Global().Get("FastTransparency", 1) };
    for (const auto& vgIt : _mesh.GetGeometries()) {
        const auto vg{ vgIt.first };
        const auto& material { vgIt.second };
        if (nullptr == vg)
            continue;
        if (nullptr == material) {
            errorLog("Error : Geometry " + vg->GetName() + " has no Material");
            continue;
        }
        const auto isTransparent(material->GetOpacityMode() == Material::OpacityMode::Blend);
        if (options.mode == Renderer::Options::Mode::Opaque && isTransparent)
            continue;
        else if (options.mode == Renderer::Options::Mode::Transparent && !isTransparent)
            continue;
        std::shared_ptr<Shader::Program> shader { material->GetShader(options.pass) };
        if (nullptr == shader)
            continue;
        material->Bind(options.pass);
        shader->Use()
            .SetUniform("DrawID", unsigned(vg->GetId()))
            .SetUniform("PrevMatrix.Model", prevTransform)
            .SetUniform("Matrix.Model", transformMatrix)
            .SetUniform("Matrix.Normal", normal_matrix)
            .SetUniform("SkinTransform", skinTransform)
            .SetUniform("SkinPrevTransform", skinPrevTransform)
            .SetTexture("Joints", skinned ? _jointMatrices.at(jointsIndex) : nullptr)
            .SetTexture("PrevJoints", skinned ? _jointMatrices.at(prevJointsIndex) : nullptr)
            .SetUniform("Skinned", skinned);
        if (options.pass == Options::Pass::ForwardOpaque || options.pass == Options::Pass::ForwardTransparent) {
            shader->SetTexture("BRDFLUT", options.renderer->GetDefaultBRDFLUT());
            shader->SetUniform("SH[0]", lightProbe.GetDiffuseSH().data(), lightProbe.GetDiffuseSH().size());
            shader->SetTexture("ReflectionMap", lightProbe.GetReflectionBuffer()->GetColorBuffer(0));
        }
        if (last_shader != shader) {
            //last_shader->use(false);
            //if (options.pass == Renderer::Pass::Material) {
            //    if (isTransparent && fastTransparency)
            //        shader->SetDefine("FASTTRANSPARENCY");
            //    shader->SetTexture("DiffuseTexture", Renderer::LightBuffer()->attachement(0));
            //    //shader->SetTexture("SpecularTexture", Renderer::LightBuffer()->attachement(1));
            //    shader->SetTexture("ReflectionTexture", Renderer::LightBuffer()->attachement(1));
            //    shader->SetTexture("AOTexture", Renderer::GeometryBuffer()->attachement(3));
            //    shader->SetTexture("NormalTexture", Renderer::GeometryBuffer()->attachement(4));
            //    shader->SetTexture("IDTexture", Renderer::GeometryBuffer()->attachement(5));
            //    shader->SetUniform("RenderPass", int(mod));
            //    if (isTransparent)
            //        shader->SetTexture("OpaqueDepthTexture", Renderer::OpaqueBuffer()->depth());
            //}
            last_shader = shader;
        }
        Renderer::Render(vg, material->GetDoubleSided());
    }
    last_shader->Done();
}

void MeshRenderer::OnFrameEnd(const Renderer::Options& options)
{
    auto skinned { _mesh.GetSkin() != nullptr };
    auto jointsIndex = _jointMatricesIndex;
    auto prevJointsIndex = (_jointMatricesIndex - 1) % _jointMatrices.size();
    _prevTransformMatrix = _mesh.GetGeometryTransform();
    if (skinned) {
        glDeleteSync(_drawSync[prevJointsIndex]);
        _drawSync[prevJointsIndex] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        glDeleteSync(_drawSync[jointsIndex]);
        _drawSync[jointsIndex] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }
}
};
