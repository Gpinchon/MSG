#include <Renderer/OGL/RAII/DebugGroup.hpp>

#include <GL/glew.h>

namespace TabGraph::Renderer::RAII {
DebugGroup::DebugGroup(const std::string& a_Name)
{
#ifdef _DEBUG
    glPushDebugGroup(
        GL_DEBUG_SOURCE_APPLICATION,
        std::hash<std::string> {}(a_Name),
        a_Name.size(), a_Name.c_str());
#endif
}
DebugGroup ::~DebugGroup()
{
#ifdef _DEBUG
    glPopDebugGroup();
#endif
}
}
