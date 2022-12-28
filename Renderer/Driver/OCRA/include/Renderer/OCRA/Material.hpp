#pragma once

#include <OCRA/Handle.hpp>

#include <memory>

OCRA_DECLARE_HANDLE(OCRA::PhysicalDevice);
OCRA_DECLARE_HANDLE(OCRA::Device);

namespace TabGraph::SG {
class Material;
}

namespace TabGraph::Renderer {
struct Material {
	Material(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice, const OCRA::Device::Handle& a_Device, const SG::Material&) {}
};
}