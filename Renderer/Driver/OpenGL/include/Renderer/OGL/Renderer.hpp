#pragma once

#include <Renderer/Handles.hpp>

#include <Renderer/OGL/RAII/FrameBuffer.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>

#ifdef WIN32
#include <Renderer/OGL/Win32/Context.hpp>
#include <Renderer/OGL/Win32/Window.hpp>
#endif

#include <GL/glew.h>
#include <string>

namespace TabGraph::Renderer {
struct CreateRendererInfo;
struct Impl {
    Impl(const CreateRendererInfo& a_Info);
    RAII::Window window { "DummyWindow", "DummyWindow" };
    RAII::Context context { window.hwnd };
    RAII::Wrapper<RAII::FrameBuffer> mainFrameBuffer { RAII::MakeWrapper<RAII::FrameBuffer>(context) };
    uint32_t version;
    std::string name;
};
}
