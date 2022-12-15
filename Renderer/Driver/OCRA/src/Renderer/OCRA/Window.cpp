#include <Renderer/Window.hpp>

#include <Renderer/OCRA/Renderer.hpp>

namespace TabGraph::Renderer::Window {
struct Impl {
    Impl(const Renderer::Handle& a_Renderer, const Info& a_Info) {}
    Impl(const Renderer::Handle& a_Renderer, void* a_NativeHandle) {}
};
/**
* Creates a window and initialized the API for it
*/
Handle Create(const Renderer::Handle& a_Renderer, const Info& a_Info) {
    return Handle(new Impl(a_Renderer, a_Info));
}
/**
* Initialized the API for an existing window
*/
Handle CreateFromNativeHandle(const Renderer::Handle& a_Renderer, void* a_NativeHandle) {
    return Handle(new Impl(a_Renderer, a_NativeHandle));
}
}