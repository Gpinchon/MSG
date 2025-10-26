#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <array>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////
namespace Msg {
class Material;
}

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
/** @brief Defines the maximum amount of materials per MaterialSet */
#define MSG_MAX_MATERIALS 64
namespace Msg {
using MaterialsArray = std::array<std::shared_ptr<Material>, MSG_MAX_MATERIALS>;
/**
 * @brief This is a component used to specify the materials for an entity
 * Usually indexed into by meshes or raytraced primitives
 */
class MaterialSet {
public:
    static constexpr size_t NoMaterial = -1u;
    MaterialSet();
    MaterialSet(const MaterialSet&) = default;
    bool Contains(const Material* a_Material) const;
    /**
     * @brief Returns the index of the specified material if it is contained in this set
     * @attention Result varies relative to current variant
     * @return The index of the material or NoMaterial if this material is not in the set
     */
    size_t GetMaterialIndex(const Material* a_Material) const;
    /**
     * @brief Returns the material at the specified index
     * @param a_Index the index of the material to fetch
     */
    const std::shared_ptr<Material>& operator[](const uint8_t& a_Index) const;
    std::shared_ptr<Material>& operator[](const uint8_t& a_Index);
    MaterialsArray materials;
};
}
