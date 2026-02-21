#define GLM_FORCE_SWIZZLE

#include <MSG/Renderer/OGL/VirtualTexture.hpp>
#include <MSG/Renderer/OGL/VirtualTextureAllocator.hpp>

#include <MSG/Debug.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/Texture.hpp>

#include <VirtualTexturing.glsl>

#include <gl/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/packing.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

template <typename T>
static inline T Round(const T& numToRound, const T& multiple)
{
    return static_cast<T>(std::round(static_cast<double>(numToRound) / static_cast<double>(multiple)) * static_cast<double>(multiple));
}

auto CreatePages(Msg::OGLContext& a_Ctx, const Msg::OGLTexture2DInfo& a_Info)
{
    Msg::OGLTexture2DInfo info;
    info.width       = Round(a_Info.width, uint32_t(VT_PAGE_SIZE)) / VT_PAGE_SIZE;
    info.height      = Round(a_Info.height, uint32_t(VT_PAGE_SIZE)) / VT_PAGE_SIZE;
    info.levels      = a_Info.levels;
    info.sizedFormat = GL_RGBA32UI;
    return std::make_shared<Msg::OGLTexture2D>(a_Ctx, info);
}

Msg::Renderer::VirtualTexture::VirtualTexture(OGLContext& a_Ctx,
    const OGLTexture2DInfo& a_Info,
    const std::shared_ptr<Texture>& a_Texture,
    const std::shared_ptr<VirtualTextureAllocator>& a_Allocator)
    : _texInfo(a_Info)
    , _texture(a_Texture)
    , _textureAllocator(a_Allocator)
    , _pagesTexture(CreatePages(a_Ctx, a_Info))
{
    auto width          = Round(a_Info.width, uint32_t(VT_PAGE_SIZE)) / VT_PAGE_SIZE;
    auto height         = Round(a_Info.height, uint32_t(VT_PAGE_SIZE)) / VT_PAGE_SIZE;
    uint32_t pixelCount = 0;
    for (uint32_t level = 0; level < a_Info.levels; level++) {
        pixelCount += width * height;
        width /= 2;
        height /= 2;
    }
    _localPages.resize(pixelCount);
}

Msg::Renderer::VirtualTexture::~VirtualTexture()
{
    for (auto& commitedPage : _commitedPages) {
        auto page = _localPages[commitedPage].page;
        page.texturePool->FreePage(page.address);
    }
}

void Msg::Renderer::VirtualTexture::RequestPage(const glm::uvec2& a_Page, const uint32_t& a_Level)
{
    auto pageIndex = _GetPageIndex(a_Page, a_Level);
    _requestedPages.insert(pageIndex);
    auto itr = _requestedPages.find(pageIndex);
    if (itr == _requestedPages.end()) {
        _requestedPages.insert(pageIndex);
        _localPages.at(pageIndex).accessTime = std::chrono::system_clock::now();
    }
}

void Msg::Renderer::VirtualTexture::Update()
{
    if (!_requestedPages.empty()) {
        for (auto& requestedPage : _requestedPages) {
            if (_commitedPages.contains(requestedPage))
                continue; // we already commited this one
            auto& page      = _localPages.at(requestedPage).page;
            auto pageCoord  = _GetPageCoordLevel(requestedPage);
            auto pixelSize  = glm::uvec3(VT_PAGE_SIZE, VT_PAGE_SIZE, 1);
            auto pixelCoord = pageCoord * pixelSize;
            _CommitPage(pageCoord.xy, pageCoord.z);
            _UploadPage(pageCoord.xy, pageCoord.z,
                std::move(_texture->at(pageCoord.z)->Read(pixelCoord, pixelSize)));
        }
        _requestedPages.clear();
    }
}

void Msg::Renderer::VirtualTexture::Cleanup()
{
    if (!_commitedPages.empty()) {
        auto now = std::chrono::system_clock::now();
        for (auto& commitedPage : _commitedPages) {
            auto& localPage = _localPages.at(commitedPage);
            if (now - localPage.accessTime > VirtualTexturePageLifeExpetency) {
                auto pageCoord = _GetPageCoordLevel(commitedPage);
                _UncommitPage(pageCoord.xy, pageCoord.z);
            }
        }
    }
}

void Msg::Renderer::VirtualTexture::_CommitPage(const glm::uvec2& a_Page, const uint32_t& a_Level)
{
    auto pageIndex = _GetPageIndex(a_Page, a_Level);
    auto itr       = _commitedPages.find(pageIndex);
    if (itr == _commitedPages.end()) {
        _commitedPages.insert(pageIndex);
        auto& localPage    = _localPages.at(pageIndex);
        localPage.page     = _textureAllocator->RequestPage(_texInfo.sizedFormat);
        localPage.commited = true;
    }
}

void Msg::Renderer::VirtualTexture::_UncommitPage(const glm::uvec2& a_Page, const uint32_t& a_Level)
{
    auto pageIndex = _GetPageIndex(a_Page, a_Level);
    auto itr       = _commitedPages.find(pageIndex);
    if (itr != _commitedPages.end()) {
        _commitedPages.erase(itr);
        auto& localPage = _localPages.at(pageIndex);
        _textureAllocator->FreePage(_texInfo.sizedFormat, localPage.page);
        localPage.page     = {};
        localPage.commited = false;
    }
}

void Msg::Renderer::VirtualTexture::_UploadPage(const glm::uvec2& a_Page, const uint32_t& a_Level, std::vector<std::byte> a_Data)
{
    auto pageIndex        = _GetPageIndex(a_Page, a_Level);
    auto& page            = _localPages.at(pageIndex).page;
    glm::uvec4 pagePixVal = {
        glm::unpackUint2x32(page.texturePool->GetID()),
        page.address
    };
    OGLTextureUploadInfo uploadInfo;
    uploadInfo.width           = VT_PAGE_SIZE;
    uploadInfo.height          = VT_PAGE_SIZE;
    uploadInfo.depth           = 1;
    uploadInfo.offsetX         = a_Page.x;
    uploadInfo.offsetY         = a_Page.y;
    uploadInfo.level           = a_Level;
    uploadInfo.pixelDescriptor = Msg::PixelSizedFormat::Uint32_RGBA;
    _pagesTexture->UploadLevel(uploadInfo, { (std::byte*)&pagePixVal[0], (std::byte*)&pagePixVal[3] });
    page.texturePool->UploadPage(page.address, std::move(a_Data));
}

uint32_t Msg::Renderer::VirtualTexture::_GetPageIndex(const glm::uvec2& a_PixelCoord, const uint32_t& a_Level) const
{
    auto width     = Round(_texInfo.width, uint32_t(VT_PAGE_SIZE)) / VT_PAGE_SIZE;
    auto height    = Round(_texInfo.height, uint32_t(VT_PAGE_SIZE)) / VT_PAGE_SIZE;
    uint32_t index = 0;
    // compute the offset of the requested level
    for (uint32_t level = 0; level < a_Level; level++) {
        index += width * height;
        width  = glm::max(1u, width / 2);
        height = glm::max(1u, height / 2);
    }
    index += width * a_PixelCoord.y + a_PixelCoord.x;
    return index;
}

glm::uvec3 Msg::Renderer::VirtualTexture::_GetPageCoordLevel(const uint32_t& a_Index) const
{
    auto remainingI = a_Index;
    auto width      = Round(_texInfo.width, uint32_t(VT_PAGE_SIZE)) / VT_PAGE_SIZE;
    auto height     = Round(_texInfo.height, uint32_t(VT_PAGE_SIZE)) / VT_PAGE_SIZE;
    // figure out on which level this index is
    for (uint32_t level = 0; level < _texInfo.levels; level++) {
        auto pixCount = width * height;
        if (pixCount > remainingI) {
            auto x = remainingI / width;
            auto y = remainingI % width;
            return { x, y, level };
        }
        remainingI -= pixCount;
        width  = glm::max(1u, width / 2);
        height = glm::max(1u, height / 2);
    }
    MSGErrorFatal("Incorrect pixel index !");
    return glm::uvec3();
}
