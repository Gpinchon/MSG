#pragma once

#include <cstdint>
#include <memory>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include <glm/vec2.hpp>

namespace Msg {
class OGLContext;
class OGLTexture;
class OGLBindlessTextureSampler;
class PixelDescriptor;
}

namespace Msg::Renderer {
constexpr glm::uvec2 VTNoPage = glm::uvec2(-1u, -1u);
class VTPool {
public:
    VTPool(OGLContext& a_Ctx, const uint32_t& a_SizedFormat);
    std::shared_ptr<OGLTexture> GetAtlas() const;
    glm::uvec2 RequestPage();
    void ReleasePage(const glm::uvec2& a_PageCoords);
    void UploadPage(const glm::uvec2& a_PageCoords, void* const a_Data);
    bool Empty() const { return _freePages.empty(); }

private:
    std::shared_ptr<OGLTexture> _atlas;
    std::queue<glm::uvec2> _freePages;
};
}