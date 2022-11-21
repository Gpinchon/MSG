#include <SG/Material/Material.hpp>

namespace TabGraph::SG {
Material::Material() : Inherit()
{
	static size_t s_MaterialNbr = 0;
	SetName("Material_" + std::to_string(s_MaterialNbr));
}
}