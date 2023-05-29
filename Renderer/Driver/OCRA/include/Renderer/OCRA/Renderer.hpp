#pragma once

#include <Renderer/Handles.hpp>

#include <OCRA/OCRA.hpp>

#include <map>

namespace TabGraph::SG {
class Primitive;
class Material;
class Scene;
}

namespace TabGraph::Renderer {
struct Primitive;
struct Material;
struct Impl {
    Impl(const OCRA::ApplicationInfo& a_Info);
    ~Impl();
    void Load(const SG::Scene& a_Scene);
    void Render(
        const SG::Scene& a_Scene,
        const RenderBuffer::Handle& a_Buffer);
    void Update();
    OCRA::Instance::Handle instance;
    OCRA::PhysicalDevice::Handle physicalDevice;
    OCRA::Device::Handle logicalDevice;
    OCRA::Command::Pool::Handle commandPool;
    OCRA::Command::Buffer::Handle commandBuffer;
    OCRA::Queue::Handle queue;
    std::map<SG::Primitive*, std::shared_ptr<Primitive>> primitives;
    std::map<SG::Material*, std::shared_ptr<Material>> materials;
};
}
