#pragma once

#include <Bindings.glsl>
#include <VTFS.glsl>

#include <array>
#include <vector>

namespace MSG {
class Scene;
class OGLBuffer;
class OGLTextureCubemap;
class OGLProgram;
}

namespace MSG::Renderer {
class Impl;
}

template <typename T, size_t... Is>
std::array<T, sizeof...(Is)> MakeArrayHelper(
    const T& val, std::index_sequence<Is...>)
{
    return { (static_cast<void>(Is), val)... };
}

template <typename T, size_t N, typename... Args>
std::array<T, N> MakeArray(Args&&... args)
{
    return MakeArrayHelper<T>(T(std::forward<Args>(args)...), std::make_index_sequence<N> {});
}

namespace MSG::Renderer {
struct GPULightCullerVTFSBuffers {
    GPULightCullerVTFSBuffers(Renderer::Impl& a_Renderer);
    std::shared_ptr<OGLBuffer> lightsBuffer; // GLSL::VTFSLightsBuffer
    std::shared_ptr<OGLBuffer> cluster; // GLSL::VTFSCluster * VTFS_CLUSTER_COUNT
    GLSL::VTFSLightsBuffer lightsBufferCPU;
};
struct GPULightCullerIBL {
    void clear()
    {
        lights.clear();
        samplers.clear();
    }
    std::vector<GLSL::LightIBL> lights;
    std::vector<std::shared_ptr<OGLTextureCubemap>> samplers;
};
constexpr auto GPULightCullerBufferNbr = 2;
class GPULightCuller {
public:
    explicit GPULightCuller(Renderer::Impl& a_Renderer);
    void operator()(Scene* a_Scene);

private:
    Renderer::Impl& _renderer;
    uint32_t _currentLightBuffer = 0;
    std::shared_ptr<OGLProgram> _vtfsCullingProgram;
    std::array<GPULightCullerVTFSBuffers, GPULightCullerBufferNbr> _vtfs = MakeArray<GPULightCullerVTFSBuffers, GPULightCullerBufferNbr>(_renderer);
    std::array<GPULightCullerIBL, GPULightCullerBufferNbr> _ibl;

public:
    GPULightCullerVTFSBuffers& vtfs;
    GPULightCullerIBL& ibl;
};
}
