#pragma once

#include <chrono>
#include <memory>
#include <unordered_map>

namespace Msg::Renderer {
class Impl;
}

namespace Msg {
class OGLFrameBuffer;
class OGLProgram;
class OGLCmdBuffer;
class OGLSampler;
class OGLTexture;
template <typename>
class OGLTypedBufferArray;
}

namespace Msg::Renderer::GLSL {
struct GaussianBlurSettingsUBO;
}

namespace Msg::Renderer {
struct TextureBlurHelper {
    TextureBlurHelper(Msg::Renderer::Impl& a_Renderer, const std::shared_ptr<Msg::OGLTexture>& a_Target);
    void operator()(Msg::Renderer::Impl& a_Renderer, Msg::OGLCmdBuffer& a_CmdBuffer, const float& a_Radius);
    std::shared_ptr<Msg::OGLProgram> shader;
    std::shared_ptr<Msg::OGLTexture> tempTexture;
    std::shared_ptr<Msg::OGLSampler> sampler;
    std::vector<std::shared_ptr<Msg::OGLFrameBuffer>> fbs;
    std::shared_ptr<Msg::OGLTypedBufferArray<GLSL::GaussianBlurSettingsUBO>> settingsBuffer;
};

class TextureBlurHelpers {
public:
    TextureBlurHelper& Get(Msg::Renderer::Impl& a_Renderer, const std::shared_ptr<Msg::OGLTexture>& a_Target)
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
        std::vector<const Msg::OGLTexture*> toErase;
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
    std::unordered_map<const Msg::OGLTexture*, std::chrono::time_point<std::chrono::steady_clock>> _lastRequests;
    std::unordered_map<const Msg::OGLTexture*, TextureBlurHelper> _blurHelpers;
};
}