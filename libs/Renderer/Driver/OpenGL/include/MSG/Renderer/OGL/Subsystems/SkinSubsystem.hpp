#pragma once

#include <MSG/Renderer/OGL/Subsystems/SubsystemInterface.hpp>

namespace MSG::Renderer {
class SkinSubsystem : public SubsystemInterface {
public:
    void Update(Renderer::Impl& a_Renderer, const SubsystemLibrary& a_Subsystems) override;
};
}