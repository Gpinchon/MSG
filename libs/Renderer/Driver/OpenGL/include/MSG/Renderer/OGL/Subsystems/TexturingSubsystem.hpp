#pragma once

#include <MSG/Renderer/OGL/Subsystems/SubsystemInterface.hpp>

#include <MSG/OGLCmdBuffer.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLFence.hpp>

#include <MSG/PageFile.hpp>

#include <VirtualTexturing.glsl>

#include <chrono>
#include <gcem.hpp>
#include <memory>
#include <queue>
#include <unordered_map>

#include <glm/vec2.hpp>

namespace MSG {
class OGLContext;
class OGLTexture2DArray;
class OGLFrameBuffer;
class OGLProgram;
template <typename>
class OGLTypedBufferArray;
}

namespace MSG::Renderer {
class VirtualTexture;
}

namespace MSG::Renderer {
constexpr std::chrono::seconds VTMaxLastAccess = std::chrono::seconds(10);

constexpr size_t VTPageBorder      = 4; // 4 texels border
constexpr size_t VTPagePixelSize   = 4; // RGBA
constexpr size_t VTPageTexelsCount = 128;
constexpr size_t VTPageLayers      = 32;
constexpr size_t VTPhysTexelsCount = 8192;
constexpr size_t VTPhysPageCount   = VTPhysTexelsCount / VTPageTexelsCount;
constexpr float VTPageFracScale    = (VTPageTexelsCount - 2.0f * VTPageBorder) / (VTPageTexelsCount * VTPhysPageCount);
struct VTPage {
    float scaleST;
    float derivScale;
    float biasS;
    float biasT;
};

class TexturingSubsystem : public SubsystemInterface {
public:
    TexturingSubsystem(Renderer::Impl& a_Renderer);
    size_t GetTileIndex(const glm::uvec3& a_Tile) const { return (a_Tile.z * VTPhysTexelsCount * VTPhysTexelsCount) + (a_Tile.y * VTPhysTexelsCount) + a_Tile.z; }
    bool TileIsBusy(const glm::uvec3& a_Tile) const;
    std::vector<glm::uvec3> RequestTiles(const size_t& a_Count);
    void FreeTile(const glm::uvec3& a_Tile);
    VTPage GetVTPage(const uint32_t& a_VirtSize, const glm::uvec2& a_VirtPage, const uint32_t& a_VirtMip, const glm::uvec2& a_PhysPage)
    {
        VTPage page;
        auto virtPagesCount    = VTPageTexelsCount / a_VirtSize;
        auto virtLvlPagesCount = virtPagesCount >> a_VirtMip;
        page.scaleST           = virtLvlPagesCount * (VTPageTexelsCount - 2.f * VTPageBorder) / float(VTPhysTexelsCount);
        page.biasS             = (a_PhysPage.x * VTPageTexelsCount + VTPageBorder) / VTPhysTexelsCount - page.scaleST * a_VirtPage.x / virtLvlPagesCount;
        page.biasT             = (a_PhysPage.y * VTPageTexelsCount + VTPageBorder) / VTPhysTexelsCount - page.scaleST * a_VirtPage.y / virtLvlPagesCount;
        page.derivScale        = VTPageFracScale * virtLvlPagesCount;
        // auto texelPhysX        = a_PhysPage.x / (VTPhysPageCount - 1);
        // auto texelPhysY        = a_PhysPage.y / (VTPhysPageCount - 1);
        return page;
    }
    void Update(Renderer::Impl& a_Renderer, const SubsystemLibrary& a_Subsystems) override;

    OGLContext ctx; // we need to place it here for destruction order

    std::shared_ptr<OGLTypedBufferArray<GLSL::VTMaterialInfo>> feedbackMaterialsBuffer;
    std::shared_ptr<OGLTypedBufferArray<GLSL::VTFeedbackOutput>> feedbackOutputBuffer;

private:
    OGLFence _feedbackFence;
    OGLCmdBuffer _feedbackCmdBuffer;
    std::shared_ptr<OGLProgram> _feedbackProgram;
    std::shared_ptr<OGLFrameBuffer> _feedbackFB;

    std::vector<std::chrono::time_point<std::chrono::system_clock>> _accessTime;
    std::queue<glm::uvec3> _freeTiles;
    std::shared_ptr<OGLTexture2DArray> _pages;
    std::shared_ptr<OGLTexture2DArray> _tiles;
};
}