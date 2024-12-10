#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <ECS/EntityRef.hpp>

#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class CullResult {
public:
    typedef ECS::DefaultRegistry::EntityRefType VisibleEntity;
    struct VisibleMesh : VisibleEntity {
        VisibleMesh(const VisibleEntity& a_Entity, const uint8_t& a_Lod = 0)
            : VisibleEntity(a_Entity)
            , lod(a_Lod)
        {
        }
        uint8_t lod = 0;
    };
    CullResult()
    {
        entities.reserve(4096);
        meshes.reserve(4096);
        skins.reserve(4096);
        lights.reserve(4096);
    }
    std::vector<VisibleEntity> entities;
    std::vector<VisibleMesh> meshes; // a subset of entities containing mesh components
    std::vector<VisibleEntity> skins; // a subset of entities containing skin components
    std::vector<VisibleEntity> lights; // a subset of entities containing light components
};
}