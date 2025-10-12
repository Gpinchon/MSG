#pragma once

#include <MSG/Renderer/SubsystemInterface.hpp>

namespace Msg::Renderer {
class TransformSubsystem : public SubsystemInterface {
public:
    void Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems) override;
};
}