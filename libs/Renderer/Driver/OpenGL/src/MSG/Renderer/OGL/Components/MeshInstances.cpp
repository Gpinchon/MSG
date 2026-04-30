#include <MSG/Renderer/OGL/Components/MeshInstances.hpp>

#include <MSG/OGLTypedBuffer.hpp>

#include <Transform.glsl>

Msg::Renderer::MeshInstances::MeshInstances(OGLContext& a_Ctx, const uint32_t& a_Instances)
    : instances(a_Instances)
    , transformBuffer(std::make_shared<OGLTypedBufferArray<GLSL::TransformUBO>>(a_Ctx, instances))
{
}