#pragma once

#include <MSG/Renderer/OGL/VirtualTextureAllocator.hpp>

#include <Msg/OGLTexture2D.hpp>

#include <chrono>
#include <memory>
#include <unordered_set>
#include <vector>

#include <glm/fwd.hpp>

namespace Msg {
class Texture;
class OGLContext;
class OGLTexture;
struct OGLTexture2DInfo;
}

namespace Msg::Renderer {
struct VirtualTexturePage;
class VirtualTextureAllocator;
}

namespace Msg::Renderer {
constexpr auto VirtualTexturePageLifeExpetency = std::chrono::seconds(30);
struct VirtualTextureLocalPage {
    VirtualTexturePage page;
    bool commited = false;
    std::chrono::system_clock::time_point accessTime;
};
class VirtualTexture {
public:
    VirtualTexture(OGLContext& a_Ctx,
        const OGLTexture2DInfo& a_Info,
        const std::shared_ptr<Texture>& a_Texture,
        const std::shared_ptr<VirtualTextureAllocator>& a_Allocator);
    ~VirtualTexture();
    /** @brief adds the specified page to the requested page list for later Update call */
    void RequestPage(const glm::uvec2& a_Page, const uint32_t& a_Level);
    /** @brief commits and uploads necessary pages */
    void Update();
    /** @brief frees pages who's access time exceeds VirtualTexturePageLifeExpetency */
    void Cleanup();

private:
    /** @brief requests necessary memory from the texture allocator */
    void _CommitPage(const glm::uvec2& a_Page, const uint32_t& a_Level);
    /** @brief restitutes the memory to the texture allocator */
    void _UncommitPage(const glm::uvec2& a_Page, const uint32_t& a_Level);
    /** @brief upload both the page address to _pagesTexture and the texture data to the texture pool */
    void _UploadPage(const glm::uvec2& a_Page, const uint32_t& a_Level, std::vector<std::byte> a_Data);
    /** @brief 2D + Level to 1D */
    uint32_t _GetPageIndex(const glm::uvec2& a_PixelCoord, const uint32_t& a_Level) const;
    /** @brief 1D to 2D + Level */
    glm::uvec3 _GetPageCoordLevel(const uint32_t& a_Index) const;
    OGLTexture2DInfo _texInfo;
    std::unordered_set<uint32_t> _requestedPages;
    std::unordered_set<uint32_t> _commitedPages;
    std::vector<VirtualTextureLocalPage> _localPages;
    std::shared_ptr<Texture> _texture;
    std::shared_ptr<VirtualTextureAllocator> _textureAllocator;
    std::shared_ptr<OGLTexture> _pagesTexture;
};
}