#include <MSG/Renderer/OGL/VirtualTextureAllocator.hpp>

#include <MSG/Debug.hpp>
#include <MSG/OGLBindlessTextureSampler.hpp>
#include <MSG/OGLTexture2D.hpp>

#include <VirtualTexturing.glsl>

Msg::Renderer::VirtualTexturePool::VirtualTexturePool(Msg::OGLContext& a_Ctx, const uint32_t& a_SizedFormat)
{
    OGLTexture2DInfo info {
        .width       = VT_PAGE_SIZE * VT_POOL_PAGE_COUNT,
        .height      = VT_PAGE_SIZE * VT_POOL_PAGE_COUNT,
        .sizedFormat = a_SizedFormat
    };
    _texture        = std::make_shared<OGLTexture2D>(a_Ctx, info);
    _textureSampler = std::make_shared<OGLBindlessTextureSampler>(a_Ctx, _texture, nullptr);
    for (uint32_t y = 0; y < VT_POOL_PAGE_COUNT; y++) {
        for (uint32_t x = 0; x < VT_POOL_PAGE_COUNT; x++) {
            _freePages.push({ x, y });
        }
    }
}

uint64_t Msg::Renderer::VirtualTexturePool::GetID()
{
    return _textureSampler->handle;
}

void Msg::Renderer::VirtualTexturePool::UploadPage(const glm::uvec2& a_Page, const std::vector<std::byte>& a_Data)
{
    OGLTextureUploadInfo info {
        .offsetX = a_Page.x * VT_PAGE_SIZE,
        .offsetY = a_Page.y * VT_PAGE_SIZE,
        .offsetZ = 0,
        .width   = VT_PAGE_SIZE,
        .height  = VT_PAGE_SIZE,
    };
    _texture->UploadLevel(info, a_Data);
}

Msg::Renderer::VirtualTexturePage Msg::Renderer::VirtualTexturePool::RequestPage()
{
    glm::uvec2 page = _freePages.front();
    _freePages.pop();
    return { this, page };
}

Msg::Renderer::VirtualTextureFormatAllocator::VirtualTextureFormatAllocator(OGLContext& a_OGLContext, const uint32_t& a_Format)
    : _format(a_Format)
    , _ctx(a_OGLContext)
{
}

Msg::Renderer::VirtualTexturePage Msg::Renderer::VirtualTextureFormatAllocator::RequestPage()
{
    if (_freePools.empty()) {
        MSGDebugStream << "Virtual textures: Allocator out of space, allocating new pool !\n";
        VirtualTexturePool newPool(_ctx, _format);
        _freePools.insert(newPool.GetID());
        _pools.insert({ newPool.GetID(), std::move(newPool) });
    }
    return _pools.at(*_freePools.begin()).RequestPage();
}

void Msg::Renderer::VirtualTextureFormatAllocator::FreePage(const VirtualTexturePage& a_Page)
{
    auto poolID = a_Page.texturePool->GetID();
    _pools.at(poolID).FreePage(a_Page.address);
    _freePools.insert(poolID);
}

void Msg::Renderer::VirtualTextureFormatAllocator::Shrink()
{
    auto itr = _freePools.begin();
    while (itr != _freePools.end()) {
        if (_pools.at(*itr).Empty()) {
            MSGDebugStream << "Virtual textures: Pool empty, removing it\n";
            _pools.erase(*itr);
            itr = _freePools.erase(itr);
        } else
            itr++;
    }
}

Msg::Renderer::VirtualTextureAllocator::VirtualTextureAllocator(OGLContext& a_OGLContext)
    : _ctx(a_OGLContext)
{
}

Msg::Renderer::VirtualTexturePage Msg::Renderer::VirtualTextureAllocator::RequestPage(const uint32_t& a_SizedFormat)
{
    auto itr = _formatAllocators.find(a_SizedFormat);
    if (itr == _formatAllocators.end()) {
        MSGDebugStream << "Virtual textures: Missing format allocator !\n";
        itr = _formatAllocators.insert({ a_SizedFormat, VirtualTextureFormatAllocator(_ctx, a_SizedFormat) }).first;
    }
    return itr->second.RequestPage();
}

void Msg::Renderer::VirtualTextureAllocator::FreePage(const uint32_t& a_SizedFormat, const VirtualTexturePage& a_Page)
{
    _formatAllocators.at(a_SizedFormat).FreePage(a_Page);
}

void Msg::Renderer::VirtualTextureAllocator::Shrink()
{
    auto itr = _formatAllocators.begin();
    while (itr != _formatAllocators.end()) {
        itr->second.Shrink();
        if (itr->second.Empty()) {
            MSGDebugStream << "Virtual textures: Allocator unused, removing it\n";
            itr = _formatAllocators.erase(itr);
        } else
            itr++;
    }
}