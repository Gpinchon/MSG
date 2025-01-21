#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Renderer/Handles.hpp>
#include <Renderer/Structs.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG::Core {
struct Image;
}

////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////
namespace MSG::Renderer::RenderBuffer {
Handle Create(
    const Renderer::Handle& a_Renderer,
    const CreateRenderBufferInfo& a_Info);
void UploadImage(
    const Handle& a_TargetRenderBuffer,
    const Core::Image& a_SourceImage);
void DownloadImage(
    const Handle& a_SourceRenderBuffer,
    const Core::Image& a_TargetImage);
//@return the image corresponding to the RenderBuffer's current state
std::shared_ptr<Core::Image> GetImage(const Handle& a_RenderBuffer);
}
