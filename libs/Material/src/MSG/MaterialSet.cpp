#include <MSG/MaterialSet.hpp>

Msg::MaterialSet::MaterialSet()
{
    materials.fill(nullptr);
}

bool Msg::MaterialSet::Contains(const Material* a_Material) const
{
    return GetMaterialIndex(a_Material) != NoMaterial;
}

size_t Msg::MaterialSet::GetMaterialIndex(const Material* a_Material) const
{
    size_t index = 0;
    for (auto& material : materials) {
        if (material.get() == a_Material)
            return index;
        index++;
    }
    return -1u;
}

const std::shared_ptr<Msg::Material>& Msg::MaterialSet::operator[](const uint8_t& a_Index) const
{
    return materials.at(a_Index);
}

std::shared_ptr<Msg::Material>& Msg::MaterialSet::operator[](const uint8_t& a_Index)
{
    return materials.at(a_Index);
}