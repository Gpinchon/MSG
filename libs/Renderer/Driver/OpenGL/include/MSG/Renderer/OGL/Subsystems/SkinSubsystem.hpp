#pragma once

#include <MSG/Renderer/OGL/SubsystemInterface.hpp>

namespace MSG::Renderer {
class SkinSubsystem : public SubsystemInterface {
public:
    void Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems) override;
};
}