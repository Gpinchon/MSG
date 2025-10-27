#pragma once

#include <MSG/Renderer/SubsystemInterface.hpp>

namespace Msg::Renderer {
class SkinSubsystem : public SubsystemInterface {
public:
    void Load(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity) override;
    void Unload(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity) override;
    void Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems) override;
};
}