#include <MSG/OGLPipelineInfo.hpp>

#include <cassert>

Msg::OGLBindings& Msg::OGLBindings::operator+=(const OGLBindings& a_Other)
{
    for (uint8_t index = 0; index < images.size(); index++) {
        auto& cur = images.at(index);
        auto& in  = a_Other.images.at(index);
        if (in.texture != nullptr) {
            assert(cur.texture == nullptr && "OGLBindings colliding");
            cur = in;
        }
    }
    for (uint8_t index = 0; index < textures.size(); index++) {
        auto& cur = textures.at(index);
        auto& in  = a_Other.textures.at(index);
        if (in.texture != nullptr) {
            assert(cur.texture == nullptr && "OGLBindings colliding");
            cur = in;
        }
    }
    for (uint8_t index = 0; index < uniformBuffers.size(); index++) {
        auto& cur = uniformBuffers.at(index);
        auto& in  = a_Other.uniformBuffers.at(index);
        if (in.buffer != nullptr) {
            assert(cur.buffer == nullptr && "OGLBindings colliding");
            cur = in;
        }
    }
    for (uint8_t index = 0; index < storageBuffers.size(); index++) {
        auto& cur = storageBuffers.at(index);
        auto& in  = a_Other.storageBuffers.at(index);
        if (in.buffer != nullptr) {
            assert(cur.buffer == nullptr && "OGLBindings colliding");
            cur = in;
        }
    }
    return *this;
}