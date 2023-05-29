#pragma once

#include <OCRA/Handles.hpp>

#include <memory>

namespace TabGraph::SG {
class Material;
}

namespace TabGraph::Renderer {
struct Material {
    Material(
        const OCRA::PhysicalDevice::Handle& a_PhysicalDevice,
        const OCRA::Device::Handle& a_Device,
        const SG::Material&) { }
};
}
