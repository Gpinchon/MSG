#pragma once

#include <GL/glew.h>
#include <array>
#include <glm/glm.hpp>
#include <vector>

#include <Bindings.glsl>

namespace TabGraph::Renderer {
template <std::size_t N>
struct num {
    static const constexpr auto value = N;
};
template <class F, std::size_t... Is>
constexpr void for_(F func, std::index_sequence<Is...>)
{
    (func(num<Is> {}), ...);
}
template <std::size_t N, typename F>
constexpr void for_(F func)
{
    for_(func, std::make_index_sequence<N>());
}

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec4 tangent;
    glm::vec2 texCoord[ATTRIB_TEXCOORD_COUNT];
    glm::vec3 color;
    glm::u16vec4 joints;
    glm::vec4 weights;
    static inline constexpr auto GetAttributeDescription()
    {
        uint8_t location = 0;
        std::array<VertexAttributeDescription, ATTRIB_COUNT> attribs {};
        attribs.at(location).binding           = 0;
        attribs.at(location).location          = location;
        attribs.at(location).format.size       = decltype(position)::length();
        attribs.at(location).format.normalized = false;
        attribs.at(location).format.type       = GL_FLOAT;
        attribs.at(location).offset            = offsetof(Vertex, position);
        ++location;
        attribs.at(location).binding           = 0;
        attribs.at(location).location          = location;
        attribs.at(location).format.size       = decltype(normal)::length();
        attribs.at(location).format.normalized = true;
        attribs.at(location).format.type       = GL_FLOAT;
        attribs.at(location).offset            = offsetof(Vertex, normal);
        ++location;
        attribs.at(location).binding           = 0;
        attribs.at(location).location          = location;
        attribs.at(location).format.size       = decltype(tangent)::length();
        attribs.at(location).format.normalized = false;
        attribs.at(location).format.type       = GL_FLOAT;
        attribs.at(location).offset            = offsetof(Vertex, tangent);
        ++location;
        for_<ATTRIB_TEXCOORD_COUNT>([&location, &attribs](auto i) {
            attribs.at(location).binding           = 0;
            attribs.at(location).location          = location;
            attribs.at(location).format.size       = std::remove_reference<decltype(texCoord[i.value])>::type::length();
            attribs.at(location).format.normalized = false;
            attribs.at(location).format.type       = GL_FLOAT;
            attribs.at(location).offset            = offsetof(Vertex, texCoord) + i.value;
            ++location;
        });
        attribs.at(location).binding           = 0;
        attribs.at(location).location          = location;
        attribs.at(location).format.size       = decltype(color)::length();
        attribs.at(location).format.normalized = false;
        attribs.at(location).format.type       = GL_FLOAT;
        attribs.at(location).offset            = offsetof(Vertex, color);
        ++location;
        attribs.at(location).binding           = 0;
        attribs.at(location).location          = location;
        attribs.at(location).format.size       = decltype(joints)::length();
        attribs.at(location).format.normalized = false;
        attribs.at(location).format.type       = GL_UNSIGNED_SHORT;
        attribs.at(location).offset            = offsetof(Vertex, joints);
        ++location;
        attribs.at(location).binding           = 0;
        attribs.at(location).location          = location;
        attribs.at(location).format.size       = decltype(weights)::length();
        attribs.at(location).format.normalized = false;
        attribs.at(location).format.type       = GL_FLOAT;
        attribs.at(location).offset            = offsetof(Vertex, weights);
        ++location;
        return attribs;
    }
};
}
