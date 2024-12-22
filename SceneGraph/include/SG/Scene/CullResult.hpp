#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <ECS/EntityRef.hpp>

#include <vector>
#include <glm/mat4x4.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
typedef ECS::DefaultRegistry::EntityRefType VisibleEntity;
struct VisibleMesh : VisibleEntity {
    VisibleMesh(const VisibleEntity& a_Entity, const uint8_t& a_Lod = 0)
        : VisibleEntity(a_Entity)
        , lod(a_Lod)
    {
    }
    uint8_t lod = 0;
};

class ShadowViewport {
public:
    glm::mat4x4 projectionMatrix;
    std::vector<VisibleMesh> meshes;
};
class VisibleShadows : public VisibleEntity {
public:
    std::vector<ShadowViewport> shadowViewports;
};

struct CullSettings {
    bool cullMeshes     = true;
    bool cullMeshSkins = true;
    bool cullLights     = true;
    bool cullShadows    = true;
};

class CullResult {
public:
    void Reserve(const size_t& a_Size) {
        entities.reserve(a_Size);
        meshes.reserve(a_Size);
        skins.reserve(a_Size);
        lights.reserve(a_Size);
        shadows.reserve(a_Size);
    }
    void Clear() {
        entities.clear();
        meshes.clear();
        skins.clear();
        lights.clear();
        shadows.clear();
    }
    void Shrink()
    {
        entities.shrink_to_fit();
        meshes.shrink_to_fit();
        skins.shrink_to_fit();
        lights.shrink_to_fit();
        shadows.shrink_to_fit();
    }
    std::vector<VisibleEntity> entities;
    std::vector<VisibleMesh> meshes; // a subset of entities containing mesh components
    std::vector<VisibleEntity> skins; // a subset of meshes containing skin components
    std::vector<VisibleEntity> lights; // a subset of entities containing light components
    std::vector<VisibleShadows> shadows; // a subset of lights casting shadows
};
}