#include <MSG/OGLDebugGroup.hpp>

#include <GL/glew.h>

namespace MSG {
OGLDebugGroup::OGLDebugGroup(const std::string& a_Name)
{
    Push(a_Name);
}

OGLDebugGroup::~OGLDebugGroup()
{
    Pop();
}

void OGLDebugGroup::Push(const std::string& a_Name)
{
#ifndef NDEBUG
    glPushDebugGroup(
        GL_DEBUG_SOURCE_APPLICATION,
        std::hash<std::string> {}(a_Name),
        a_Name.size(), a_Name.c_str());
#endif
}

void OGLDebugGroup::Pop()
{
#ifndef NDEBUG
    glPopDebugGroup();
#endif
}
}
