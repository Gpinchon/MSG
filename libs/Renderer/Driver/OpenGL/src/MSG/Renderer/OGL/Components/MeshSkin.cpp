#include <MSG/Entity/Node.hpp>
#include <MSG/Mesh/Skin.hpp>
#include <MSG/OGLBuffer.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/Renderer/OGL/Components/MeshSkin.hpp>

#include <GL/glew.h>
#include <glm/mat4x4.hpp>

namespace Msg::Renderer {
MeshSkin::MeshSkin(OGLContext& a_Context, const glm::mat4x4& a_Transform, const Msg::MeshSkin& a_Skin)
{
    auto skinSize = a_Skin.joints.size() * sizeof(glm::mat4x4);
    for (auto& buffer : _buffers) {
        buffer = std::make_shared<OGLBuffer>(a_Context, skinSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        Update(a_Context, a_Transform, a_Skin); // update all buffers
    }
}

void MeshSkin::Update(OGLContext& a_Context, const glm::mat4x4& a_Transform, const Msg::MeshSkin& a_Skin)
{
    buffer_Previous = _buffers[(_bufferIndex - 1) % _buffers.size()];
    buffer          = _buffers[_bufferIndex];
    _bufferIndex    = (_bufferIndex + 1) % _buffers.size();
    std::vector<glm::mat4x4> jointsMatrix;
    auto inverseTransformMatrix = glm::inverse(a_Transform);
    jointsMatrix.resize(a_Skin.joints.size());
    for (uint32_t i = 0; i < a_Skin.joints.size(); i++) {
        auto jointMatrix        = a_Skin.joints.at(i).GetComponent<Msg::Transform>().GetWorldTransformMatrix();
        auto& inverseBindMatrix = a_Skin.inverseBindMatrices.at(i);
        jointsMatrix.at(i)      = inverseTransformMatrix * jointMatrix * inverseBindMatrix;
    }
    a_Context.PushCmd([buffer = buffer, jointsMatrix = jointsMatrix] {
        glInvalidateBufferSubData(*buffer, 0, buffer->size);
        glNamedBufferSubData(*buffer, 0, buffer->size, jointsMatrix.data());
    });
}
}
