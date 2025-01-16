#pragma once

#include <OCRA/OCRA.hpp>

OCRA_DECLARE_HANDLE(OCRA::Device);
OCRA_DECLARE_HANDLE(OCRA::Memory);
OCRA_DECLARE_HANDLE(OCRA::Buffer);

namespace TabGraph::Renderer {
OCRA::Memory::Handle AllocateMemory(
    const OCRA::PhysicalDevice::Handle& a_PhysicalDevice,
    const OCRA::Device::Handle& a_Device,
    const uint64_t& a_Size,
    const OCRA::MemoryPropertyFlags& a_MemoryProperties);
OCRA::Buffer::Handle CreateBuffer(
    const OCRA::BufferUsageFlags& a_Usage,
    const OCRA::Device::Handle& a_Device,
    const OCRA::Memory::Handle& a_Memory,
    size_t a_Size, size_t a_Offset);
void FillMemory(
    const OCRA::Memory::Handle& a_Memory,
    const void* a_Data,
    size_t a_Size, size_t a_Offset);
}
