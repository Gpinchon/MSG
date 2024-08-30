#include <Renderer/OGL/Components/MeshSkin.hpp>
#include <Renderer/OGL/RAII/Buffer.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <SG/Component/MeshSkin.hpp>
#include <SG/Entity/Node.hpp>

#include <GL/glew.h>
#include <glm/mat4x4.hpp>

namespace TabGraph::Renderer::Component {
MeshSkin::MeshSkin(Context& a_Context, const glm::mat4x4& a_Transform, const SG::Component::MeshSkin& a_Skin)
{
    auto skinSize = a_Skin.joints.size() * sizeof(glm::mat4x4);
    for (auto& buffer : _buffers) {
        buffer = RAII::MakePtr<RAII::Buffer>(a_Context, skinSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        Update(a_Context, a_Transform, a_Skin); // update all buffers
    }
}

void MeshSkin::Update(Context& a_Context, const glm::mat4x4& a_Transform, const SG::Component::MeshSkin& a_Skin)
{
    buffer_Previous = _buffers[(_bufferIndex - 1) % _buffers.size()];
    buffer          = _buffers[_bufferIndex];
    _bufferIndex    = (_bufferIndex + 1) % _buffers.size();
    std::vector<glm::mat4x4> jointsMatrix;
    auto inverseTransformMatrix = glm::inverse(a_Transform);
    jointsMatrix.resize(a_Skin.joints.size());
    for (uint32_t i = 0; i < a_Skin.joints.size(); i++) {
        auto jointMatrix        = SG::Node::GetWorldTransformMatrix(a_Skin.joints.at(i));
        auto& inverseBindMatrix = a_Skin.inverseBindMatrices.at(i);
        jointsMatrix.at(i)      = inverseTransformMatrix * jointMatrix * inverseBindMatrix;
    }
    a_Context.PushCmd([buffer = buffer, jointsMatrix = jointsMatrix] {
        glInvalidateBufferData(*buffer);
        glNamedBufferSubData(*buffer, 0, buffer->size, jointsMatrix.data());
    });
}
}
