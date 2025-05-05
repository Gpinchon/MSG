#pragma once

#include <memory>
#include <variant>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace MSG {
class OGLTexture;
struct OGLCmdBufferState;
}

namespace MSG {
using OGLClearValue = std::variant<glm::vec4, glm::ivec4, glm::uvec4>;
struct OGLClearTextureInfo {
    int level         = 0;
    glm::ivec3 offset = { 0, 0, 0 };
    glm::uvec3 size   = { 0, 0, 0 };
    OGLClearValue value;
};
class OGLCmdClearTexture {
public:
    OGLCmdClearTexture(
        const std::shared_ptr<OGLTexture>& a_Txt,
        const OGLClearTextureInfo& a_Info);
    void operator()(OGLCmdBufferState&) const;

private:
    const std::shared_ptr<OGLTexture> _txt;
    const uint32_t _level;
    const glm::ivec3 _offset;
    const glm::uvec3 _size;
    const uint32_t _type;
    const uint32_t _format;
    const OGLClearValue _value;
};
}
