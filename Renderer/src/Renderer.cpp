#include <OCRA/Instance.hpp>

#include <memory>

namespace TabGraph::Renderer {
struct Impl {
    Impl(const OCRA::Application::Info& a_Info)
        : _instance(OCRA::Instance::Create({ a_Info }))
    {}
    OCRA::Instance::Handle _instance;
    OCRA::PhysicalDevice::Handle _physicalDevice{ OCRA::Instance::EnumeratePhysicalDevices(_instance).front() };
};
typedef std::shared_ptr<Impl> Handle;
}

namespace TabGraph::Renderer {
Handle Create(const OCRA::Application::Info& a_Info) {
    return Handle(new Impl(a_Info));
}
}