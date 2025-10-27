#pragma once

#include <MSG/Renderer/SubsystemInterface.hpp>

#include <MSG/Renderer/OGL/Components/MaterialSet.hpp>
#include <MSG/Tools/ObjectCache.hpp>

namespace Msg {
class MaterialSet;
}

namespace Msg::Renderer {
using MaterialCacheKey = Tools::ObjectCacheKey<Msg::Material*>;
using MaterialCache    = Tools::ObjectCache<MaterialCacheKey, std::shared_ptr<Material>>;
class MaterialSubsystem : public SubsystemInterface {
public:
    ~MaterialSubsystem();
    void Load(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity) override;
    void Unload(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity) override;
    void Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems) override;
    MaterialCache materialCache;
};
}