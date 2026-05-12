#include <MSG/Renderer/OGL/VTPool.hpp>

#include <MSG/Debug.hpp>
#include <MSG/OGLBindlessTextureSampler.hpp>
#include <MSG/OGLTexture2D.hpp>

#include <VirtualTexturing.glsl>

void Msg::Renderer::VTPool::Allocate(Msg::OGLContext& a_Ctx, const uint32_t& a_PageCount, const uint32_t& a_SizedFormat)
{
    OGLTexture2DInfo info {
        .width       = VT_PAGE_SIZE * a_PageCount,
        .height      = VT_PAGE_SIZE * a_PageCount,
        .sizedFormat = a_SizedFormat
    };
    _atlas     = std::make_shared<OGLTexture2D>(a_Ctx, info);
    _freePages = { };
    for (uint32_t y = 0; y < a_PageCount; y++) {
        for (uint32_t x = 0; x < a_PageCount; x++) {
            _freePages.push({ x, y });
        }
    }
    _atlas->Clear(GL_RGBA, GL_UNSIGNED_BYTE, 0, &glm::u8vec4(0)[0]);
}

std::shared_ptr<Msg::OGLTexture> Msg::Renderer::VTPool::GetAtlas() const
{
    return _atlas;
}

glm::uvec2 Msg::Renderer::VTPool::RequestPage()
{
    if (_freePages.empty()) {
        if (!_warningDelivered) {
            MSGErrorWarning("Virtual Texture: Page pool no more free page available !");
            _warningDelivered = true;
        }
        return VTNoPage;
    }
    glm::uvec2 page = _freePages.front();
    _freePages.pop();
    return page;
}

void Msg::Renderer::VTPool::ReleasePage(const glm::uvec2& a_PageCoords)
{
    if (_warningDelivered) {
        MSGErrorWarning("Virtual Texture: Page freed");
        _warningDelivered = false;
    }
    _freePages.push(a_PageCoords);
}

void Msg::Renderer::VTPool::UploadPage(const glm::uvec2& a_Page, void* const a_Data)
{
    OGLTextureUploadInfo info;
    info.offsetX = a_Page.x * VT_PAGE_SIZE;
    info.offsetY = a_Page.y * VT_PAGE_SIZE;
    info.offsetZ = 0;
    info.width   = VT_PAGE_SIZE;
    info.height  = VT_PAGE_SIZE;
    info.format  = GL_RGBA;
    info.type    = GL_UNSIGNED_BYTE;
    _atlas->UploadLevel(info, a_Data);
}