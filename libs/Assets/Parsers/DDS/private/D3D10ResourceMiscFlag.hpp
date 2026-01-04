#pragma once

#include <bitset>

namespace Msg {
using D3D10ResourceMiscFlags = std::bitset<32>;
namespace D3D10ResourceMiscFlag {
    D3D10ResourceMiscFlags GenerateMips     = 0x1L;
    D3D10ResourceMiscFlags Shared           = 0x2L;
    D3D10ResourceMiscFlags TextureCube      = 0x4L;
    D3D10ResourceMiscFlags SharedKeyedMutex = 0x10L;
    D3D10ResourceMiscFlags GDICompatible    = 0x20L;
}
}
