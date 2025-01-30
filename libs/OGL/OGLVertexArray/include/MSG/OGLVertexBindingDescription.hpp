#pragma once

#include <memory>

namespace MSG {
class OGLBuffer;
}

namespace MSG {
struct OGLVertexBindingDescription {
    unsigned index  = 0;
    unsigned offset = 0;
    unsigned stride = 0;
    std::shared_ptr<OGLBuffer> buffer;
};
}
