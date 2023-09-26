#pragma once

namespace TabGraph::Renderer::RAII {
struct ProgramPipeline {
    ProgramPipeline();
    ~ProgramPipeline();
    operator unsigned() const { return handle; }
    const unsigned handle;
};
}
