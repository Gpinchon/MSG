#pragma once

#include <chrono>
#include <memory>
#include <unordered_map>

namespace MSG::Renderer {
class Impl;
}

namespace MSG {
class OGLFrameBuffer;
class OGLProgram;
class OGLCmdBuffer;
class OGLSampler;
class OGLTexture;
template <typename>
class OGLTypedBufferArray;
}

namespace MSG::Renderer::GLSL {
struct GaussianBlurSettingsUBO;
}

namespace MSG::Renderer {
struct TextureBlurHelper {
    TextureBlurHelper(MSG::Renderer::Impl& a_Renderer, const std::shared_ptr<MSG::OGLTexture>& a_Target);
    void operator()(MSG::Renderer::Impl& a_Renderer, MSG::OGLCmdBuffer& a_CmdBuffer, const float& a_Radius);
    std::shared_ptr<MSG::OGLProgram> shader;
    std::shared_ptr<MSG::OGLTexture> tempTexture;
    std::shared_ptr<MSG::OGLSampler> sampler;
    std::vector<std::shared_ptr<MSG::OGLFrameBuffer>> fbs;
    std::shared_ptr<MSG::OGLTypedBufferArray<GLSL::GaussianBlurSettingsUBO>> settingsBuffer;
};

class TextureBlurHelpers {
public:
    TextureBlurHelper& Get(MSG::Renderer::Impl& a_Renderer, const std::shared_ptr<MSG::OGLTexture>& a_Target)
    {
        auto itr = _blurHelpers.find(a_Target.get());
        if (itr == _blurHelpers.end()) {
            itr = _blurHelpers.insert({ a_Target.get(), TextureBlurHelper(a_Renderer, a_Target) }).first;
        }
        _lastRequests[itr->first] = std::chrono::steady_clock::now();
        return itr->second;
    }
    void Update()
    {
        std::vector<const MSG::OGLTexture*> toErase;
        toErase.reserve(_lastRequests.size());
        auto now = std::chrono::steady_clock::now();
        for (auto& lastRequest : _lastRequests) {
            auto duration = now - lastRequest.second;
            if (duration >= std::chrono::seconds(10)) // erase the helper if it hasn't been requested for 10 seconds
                toErase.emplace_back(lastRequest.first);
        }
        for (auto& ptr : toErase) {
            _lastRequests.erase(ptr);
            _blurHelpers.erase(ptr);
        }
    };

private:
    std::unordered_map<const MSG::OGLTexture*, std::chrono::time_point<std::chrono::steady_clock>> _lastRequests;
    std::unordered_map<const MSG::OGLTexture*, TextureBlurHelper> _blurHelpers;
};
}