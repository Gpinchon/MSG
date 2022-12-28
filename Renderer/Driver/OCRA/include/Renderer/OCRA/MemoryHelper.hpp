#pragma once

#include <OCRA/Handle.hpp>
#include <OCRA/PhysicalDevice.hpp>

OCRA_DECLARE_HANDLE(OCRA::Device);
OCRA_DECLARE_HANDLE(OCRA::Memory);
OCRA_DECLARE_HANDLE(OCRA::Buffer);

namespace TabGraph::Renderer {
uint32_t FindProperMemoryType(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice, const OCRA::PhysicalDevice::MemoryPropertyFlags& a_MemoryProperties);
OCRA::Memory::Handle AllocateMemory(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice, const OCRA::Device::Handle& a_Device, const uint64_t& a_Size, const OCRA::PhysicalDevice::MemoryPropertyFlags& a_MemoryProperties);
OCRA::Buffer::Handle CreateBuffer(const OCRA::Device::Handle& a_Device, const OCRA::Memory::Handle& a_Memory, size_t a_Size, size_t a_Offset);
void FillMemory(const OCRA::Device::Handle& a_Device, const OCRA::Memory::Handle& a_Memory, const void* a_Data, size_t a_Size, size_t a_Offset);
}