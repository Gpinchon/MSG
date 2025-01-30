#include <MSG/OGLDebugGroup.hpp>

#include <GL/glew.h>

namespace MSG {

OGLDebugGroup::OGLDebugGroup(const std::string& a_Name)
{
#ifndef NDEBUG
    glPushDebugGroup(
        GL_DEBUG_SOURCE_APPLICATION,
        std::hash<std::string> {}(name),
        name.size(), name.c_str());
#endif
}

OGLDebugGroup::~OGLDebugGroup()
{
#ifndef NDEBUG
    glPopDebugGroup();
#endif
}
}
