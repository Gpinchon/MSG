#pragma once

#include <MSG/OGLRenderPass/Info.hpp>

namespace MSG {
class OGLRenderPass {
public:
    OGLRenderPass(const OGLRenderPassInfo& a_Info);
    void Execute() const;
    const OGLRenderPassInfo info;
};
}
