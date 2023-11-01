#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Image.hpp>
#include <SG/Core/Material.hpp>
#include <SG/Core/Texture/Sampler.hpp>
#include <SG/Core/Texture/Texture.hpp>

#include <cassert>
#include <functional>
#include <glm/glm.hpp>
#include <iostream>

namespace TabGraph::SG {
static size_t s_MaterialNbr = 0;
Material::Material()
    : Inherit("Material_" + std::to_string(s_MaterialNbr))
{
}
}
