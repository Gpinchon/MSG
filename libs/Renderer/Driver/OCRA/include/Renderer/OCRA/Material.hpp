#pragma once

#include <MSG/Renderer/Handles.hpp>

#include <OCRA/Structs.hpp>

#include <memory>

namespace MSG::SG {
class Material;
}

namespace MSG::Renderer {
struct Material {
    Material(
        const Renderer::Impl& a_Renderer,
        const Material& a_Material);
};
}
