#pragma once

#include <MSG/OGLCmdBuffer.hpp>
#include <MSG/OGLFence.hpp>
#include <MSG/Renderer/SubsystemInterface.hpp>

#include <Bindings.glsl>
#include <LightsVTFS.glsl>

namespace Msg {
enum class LightType;
class OGLContext;
class OGLBindlessTextureSampler;
template <typename>
class OGLTypedBuffer;
template <typename>
class OGLTypedBufferArray;
class OGLProgram;
}

namespace Msg::Renderer {
class Impl;
}

namespace Msg::Renderer {
int GetGLSLLightType(const LightType& a_LightType);

class LightsVTFSBuffer {
public:
    LightsVTFSBuffer(OGLContext& a_Ctx);
    LightsVTFSBuffer(const LightsVTFSBuffer&) = delete;
    LightsVTFSBuffer(LightsVTFSBuffer&&)      = default;
    std::shared_ptr<OGLTypedBuffer<GLSL::VTFSLightsBuffer>> lightsBuffer; // GLSL::VTFSLightsBuffer
    std::shared_ptr<OGLTypedBufferArray<GLSL::VTFSCluster>> cluster; // GLSL::VTFSCluster * VTFS_CLUSTER_COUNT
    OGLFence executionFence { true };
    OGLCmdBuffer cmdBuffer;
};

constexpr size_t VTFSBufferNbr = 2;

class LightsVTFSSubsystem : public SubsystemInterface {
public:
    LightsVTFSSubsystem(Renderer::Impl& a_Renderer);
    void Load(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity) override;
    void Unload(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity) override;
    void Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems) override;
    LightsVTFSBuffer* buffer;

private:
    uint32_t _currentBuffer = 0;
    std::shared_ptr<OGLProgram> _cullingProgram;
    std::array<LightsVTFSBuffer, VTFSBufferNbr> _buffers;
};
}