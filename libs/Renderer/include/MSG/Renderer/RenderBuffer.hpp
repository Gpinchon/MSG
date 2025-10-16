#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Renderer/Handles.hpp>
#include <MSG/Renderer/Structs.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace Msg {
struct Image;
}

////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////
namespace Msg::Renderer::RenderBuffer {
Handle Create(
    const Renderer::Handle& a_Renderer,
    const CreateRenderBufferInfo& a_Info);
/**
 * @brief Get the Native handle of the RenderBuffer.
 * Could be GLuint or VkImage depending on the backend.
 *
 * @param a_RenderBuffer the render to get the native handle from
 * @return a wrapper around a GLuint/VkImage...
 */
std::any GetNativeHandle(
    const Handle& a_RenderBuffer);
void UploadImage(
    const Handle& a_TargetRenderBuffer,
    const Image& a_SourceImage);
void DownloadImage(
    const Handle& a_SourceRenderBuffer,
    const Image& a_TargetImage);
}
