#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Camera/Projection.hpp>
#include <MSG/ECS/EntityRef.hpp>

#include <glm/mat4x4.hpp>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace Msg {
struct VisibleEntity {
    typedef ECS::DefaultRegistry::EntityIDType IDType;
    VisibleEntity(const IDType& a_ID)
        : _id(a_ID)
    {
    }
    operator const IDType&() const { return _id; }

private:
    IDType _id = 0;
};
struct SceneVisibleMesh : VisibleEntity {
    SceneVisibleMesh(const VisibleEntity& a_Entity, const uint8_t& a_Lod = 0)
        : VisibleEntity(a_Entity)
        , lod(a_Lod)
    {
    }
    uint8_t lod = 0;
};

class SceneShadowViewport {
public:
    CameraProjection projection;
    glm::mat4x4 viewMatrix;
    std::vector<SceneVisibleMesh> meshes;
};
class SceneVisibleLight : public VisibleEntity {
public:
    std::vector<SceneShadowViewport> viewports; // this will be filled if this light casts shadows
};

struct SceneCullSettings {
    bool cullMeshes    = true;
    bool cullMeshSkins = true;
    bool cullLights    = true;
    bool cullShadows   = true;
    bool cullFogAreas  = true;
};

class SceneCullResult {
public:
    void Reserve(const size_t& a_Size)
    {
        entities.reserve(a_Size);
        meshes.reserve(a_Size);
        skins.reserve(a_Size);
        lights.reserve(a_Size);
        fogAreas.reserve(a_Size);
    }
    void Clear()
    {
        entities.clear();
        meshes.clear();
        skins.clear();
        lights.clear();
        fogAreas.clear();
    }
    void Shrink()
    {
        entities.shrink_to_fit();
        meshes.shrink_to_fit();
        skins.shrink_to_fit();
        lights.shrink_to_fit();
        fogAreas.shrink_to_fit();
    }
    std::vector<VisibleEntity> entities;
    std::vector<SceneVisibleMesh> meshes; // a subset of entities containing mesh components
    std::vector<VisibleEntity> skins; // a subset of meshes containing skin components
    std::vector<SceneVisibleLight> lights; // a subset of entities containing light components
    std::vector<VisibleEntity> fogAreas; // a subset of entities containing VolumetricMesh components
};
}