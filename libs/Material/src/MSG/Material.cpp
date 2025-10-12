#include <MSG/Material.hpp>

namespace Msg {
static size_t s_MaterialNbr = 0;
Material::Material()
    : Inherit("Material_" + std::to_string(s_MaterialNbr))
{
}
}
