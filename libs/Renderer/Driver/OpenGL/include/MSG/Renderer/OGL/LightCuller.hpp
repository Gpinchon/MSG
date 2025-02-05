#pragma once

#include <Bindings.glsl>
#include <VTFS.glsl>

#include <array>
#include <vector>

namespace MSG {
class Scene;
class OGLContext;
class OGLBuffer;
class OGLTextureCubemap;
class OGLProgram;
}

namespace MSG::Renderer::Component {
class LightData;
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
struct LightCullerVTFSBuffers {
    LightCullerVTFSBuffers(OGLContext& a_Ctx);
    std::shared_ptr<OGLBuffer> lightsBuffer; // GLSL::VTFSLightsBuffer
    std::shared_ptr<OGLBuffer> cluster; // GLSL::VTFSCluster * VTFS_CLUSTER_COUNT
    GLSL::VTFSLightsBuffer lightsBufferCPU;
};
struct LightCullerIBL {
    void clear()
    {
        lights.clear();
        samplers.clear();
    }
    std::vector<GLSL::LightIBL> lights;
    std::vector<std::shared_ptr<OGLTextureCubemap>> samplers;
};
constexpr auto GPULightCullerBufferNbr = 2;
class LightCuller {
public:
    explicit LightCuller(Renderer::Impl& a_Renderer, const uint32_t& a_MaxIBL, const uint32_t& a_MaxShadowCasters);
    void operator()(Scene* a_Scene, const std::shared_ptr<OGLBuffer>& a_CameraUBO);

private:
    template <typename LightType>
    void _PushLight(const LightType& a_LightData);
    OGLContext& _context;
    const uint32_t _maxIBL;
    const uint32_t _maxShadowCasters;
    uint32_t _currentLightBuffer = 0;
    std::shared_ptr<OGLProgram> _vtfsCullingProgram;
    std::array<LightCullerVTFSBuffers, GPULightCullerBufferNbr> _vtfs = MakeArray<LightCullerVTFSBuffers, GPULightCullerBufferNbr>(_context);
    std::array<LightCullerIBL, GPULightCullerBufferNbr> _ibl;

public:
    LightCullerVTFSBuffers& vtfs;
    LightCullerIBL& ibl;
};
}
