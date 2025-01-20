#pragma once

#include <OCRA/Handles.hpp>

namespace MSG::Renderer {
class IndexBuffer {
private:
    const size_t size = 0;
    const size_t offset = 0;
    const size_t indexSize = 0;
    const OCRA::Memory::Handle memory;
    const OCRA::Buffer::Handle buffer;
};
}