#pragma once

namespace TabGraph::Renderer::RAII {
struct Texture2D;
struct FrameBufferBlitInfo {
    unsigned srcX0; // src X offset
    unsigned srcY0; // src Y offset
    unsigned srcX1; // src X extent
    unsigned srcY1; // src Y extent
    unsigned dstX0; // dst X offset
    unsigned dstY0; // dst Y offset
    unsigned dstX1; // dst X extent
    unsigned dstY1; // dst Y extent
    unsigned mask; // a bitfield of the bits to blit
    unsigned filter; // Blit filter
};
struct FrameBuffer {
    FrameBuffer();
    ~FrameBuffer();
    /**
     * @brief Blits to the specified FB
     * @param a_Other The FB to blit to
     * @param a_Info the rect to blit
     */
    void Blit(
        const FrameBuffer& a_Other,
        const FrameBufferBlitInfo& a_Info);
    /**
     * @brief Blits to default FB
     * @param a_Info the rect to blit
     */
    void Blit(const FrameBufferBlitInfo& a_Info);
    /**
     * @brief Attach a Texture2D to the specified color attachment
     * @param a_Texture The Texture2D to attach
     * @param a_ColorIndex The color attachment to attach the texture to
     */
    void AttachColorTexture(const Texture2D& a_Texture, unsigned a_ColorIndex);
    /**
     * @brief Detach the specified color attachment
     * @param a_ColorIndex The color attachment to detach
     */
    void DetachColorTexture(unsigned a_ColorIndex);
    operator unsigned() const { return handle; }
    unsigned handle = 0;
};
}
