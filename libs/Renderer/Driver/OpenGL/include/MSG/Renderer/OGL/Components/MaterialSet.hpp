#pragma once

#include <MSG/MaterialSet.hpp>

#include <memory>

namespace Msg::Renderer {
class Material;
}

namespace Msg::Renderer {
using MaterialSet = std::array<std::shared_ptr<Material>, MSG_MAX_MATERIALS>;
}