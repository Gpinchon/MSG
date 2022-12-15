#include <Renderer/Renderer.hpp>

#include <Renderer/OCRA/Renderer.hpp>

#include <OCRA/Instance.hpp>

namespace TabGraph::Renderer {
Handle Create(const Info& a_Info) {
    OCRA::Application::Info info;
    info.name = a_Info.name;
    info.applicationVersion = info.applicationVersion;
    info.engineVersion = 100;
    info.engineName = "TabGraph";
    return Handle(new Impl(info));
}
}
