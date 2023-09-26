#include <Renderer/OGL/RAII/ProgramPipeline.hpp>

#include <GL/glew.h>

namespace TabGraph::Renderer::RAII {
auto CreateProgramPipeline() { 
    unsigned handle = 0;
    glCreateProgramPipelines(1, &handle);
    return handle;
}
ProgramPipeline::ProgramPipeline()
    : handle(CreateProgramPipeline())
{
}

ProgramPipeline::~ProgramPipeline()
{
    glDeleteProgramPipelines(1, &handle);
}
}
