#pragma once

#include <MSG/OGLRenderPassInfo.hpp>

namespace MSG {
class OGLRenderPass {
public:
    OGLRenderPass(const OGLRenderPassInfo& a_Info);
    void Begin() const;
    void End() const;
    const OGLRenderPassInfo info;
};
}
