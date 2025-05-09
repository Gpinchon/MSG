#pragma once

#include <MSG/Renderer/OGL/Components/LightData.hpp>
#include <MSG/Tools/MakeArrayHelper.hpp>

#include <MSG/OGLCmdBuffer.hpp>
#include <MSG/OGLFence.hpp>

#include <Bindings.glsl>
#include <LightsVTFS.glsl>

#include <array>

namespace MSG {
class Scene;
class OGLContext;
template <typename>
class OGLTypedBuffer;
template <typename>
class OGLTypedBufferArray;
class OGLProgram;
}

namespace MSG::Renderer::Component {
class LightData;
}

namespace MSG::Renderer {
class Impl;
}

namespace MSG::Renderer {
class LightCullerVTFSBuffer {
public:
    LightCullerVTFSBuffer(OGLContext& a_Ctx);
    std::shared_ptr<OGLTypedBuffer<GLSL::VTFSLightsBuffer>> lightsBuffer; // GLSL::VTFSLightsBuffer
    std::shared_ptr<OGLTypedBufferArray<GLSL::VTFSCluster>> cluster; // GLSL::VTFSCluster * VTFS_CLUSTER_COUNT
    GLSL::VTFSLightsBuffer lightsBufferCPU;
    OGLFence executionFence { true };
    OGLCmdBuffer cmdBuffer;
};

constexpr size_t VTFSBufferNbr = 2;
class LightCullerVTFS {
public:
    explicit LightCullerVTFS(Renderer::Impl& a_Renderer);
    void operator()(Scene* a_Scene, const std::shared_ptr<OGLBuffer>& a_CameraUBO);

private:
    OGLContext& _context;
    uint32_t _currentBuffer = 0;
    std::shared_ptr<OGLProgram> _cullingProgram;
    std::array<LightCullerVTFSBuffer, VTFSBufferNbr> _buffers { LightCullerVTFSBuffer(_context), LightCullerVTFSBuffer(_context) };

public:
    void Prepare();
    template <typename LightType>
    bool PushLight(const LightType&);
    void Cull(const std::shared_ptr<OGLBuffer>& a_CameraUBO);
    LightCullerVTFSBuffer* buffer;
};
}

template <typename LightType>
inline bool MSG::Renderer::LightCullerVTFS::PushLight(const LightType& a_Light)
{
    auto& lights = buffer->lightsBufferCPU;
    if (lights.count >= VTFS_BUFFER_MAX) [[unlikely]]
        return false;
    lights.lights[lights.count] = *reinterpret_cast<const GLSL::LightBase*>(&a_Light);
    lights.count++;
    return true;
}
template <>
inline bool MSG::Renderer::LightCullerVTFS::PushLight(const Component::LightData& a_LightData)
{
    return std::visit([this](auto& a_Data) mutable { return PushLight(a_Data); }, a_LightData);
}