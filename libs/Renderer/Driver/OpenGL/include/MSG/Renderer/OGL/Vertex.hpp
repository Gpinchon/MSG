#pragma once

#include <MSG/Mesh/Primitive.hpp>

#include <GL/glew.h>
#include <array>
#include <glm/glm.hpp>
#include <vector>

#include <Bindings.glsl>

namespace MSG::Renderer {
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

static inline constexpr auto GetVertexAttributeDescription()
{
    std::array<OGLVertexAttributeDescription, ATTRIB_COUNT> attribs {};
    attribs.at(ATTRIB_POSITION).binding           = 0;
    attribs.at(ATTRIB_POSITION).location          = ATTRIB_POSITION;
    attribs.at(ATTRIB_POSITION).format.size       = decltype(Vertex::position)::length();
    attribs.at(ATTRIB_POSITION).format.normalized = false;
    attribs.at(ATTRIB_POSITION).format.type       = GL_FLOAT;
    attribs.at(ATTRIB_POSITION).offset            = offsetof(Vertex, position);
    attribs.at(ATTRIB_NORMAL).binding             = 0;
    attribs.at(ATTRIB_NORMAL).location            = ATTRIB_NORMAL;
    attribs.at(ATTRIB_NORMAL).format.size         = decltype(Vertex::normal)::length();
    attribs.at(ATTRIB_NORMAL).format.normalized   = true;
    attribs.at(ATTRIB_NORMAL).format.type         = GL_FLOAT;
    attribs.at(ATTRIB_NORMAL).offset              = offsetof(Vertex, normal);
    attribs.at(ATTRIB_TANGENT).binding            = 0;
    attribs.at(ATTRIB_TANGENT).location           = ATTRIB_TANGENT;
    attribs.at(ATTRIB_TANGENT).format.size        = decltype(Vertex::tangent)::length();
    attribs.at(ATTRIB_TANGENT).format.normalized  = false;
    attribs.at(ATTRIB_TANGENT).format.type        = GL_FLOAT;
    attribs.at(ATTRIB_TANGENT).offset             = offsetof(Vertex, tangent);
    attribs.at(ATTRIB_COLOR).binding              = 0;
    attribs.at(ATTRIB_COLOR).location             = ATTRIB_COLOR;
    attribs.at(ATTRIB_COLOR).format.size          = decltype(Vertex::color)::length();
    attribs.at(ATTRIB_COLOR).format.normalized    = false;
    attribs.at(ATTRIB_COLOR).format.type          = GL_FLOAT;
    attribs.at(ATTRIB_COLOR).offset               = offsetof(Vertex, color);
    attribs.at(ATTRIB_JOINTS).binding             = 0;
    attribs.at(ATTRIB_JOINTS).location            = ATTRIB_JOINTS;
    attribs.at(ATTRIB_JOINTS).format.size         = decltype(Vertex::joints)::length();
    attribs.at(ATTRIB_JOINTS).format.normalized   = false;
    attribs.at(ATTRIB_JOINTS).format.type         = GL_FLOAT;
    attribs.at(ATTRIB_JOINTS).offset              = offsetof(Vertex, joints);
    attribs.at(ATTRIB_WEIGHTS).binding            = 0;
    attribs.at(ATTRIB_WEIGHTS).location           = ATTRIB_WEIGHTS;
    attribs.at(ATTRIB_WEIGHTS).format.size        = decltype(Vertex::weights)::length();
    attribs.at(ATTRIB_WEIGHTS).format.normalized  = true;
    attribs.at(ATTRIB_WEIGHTS).format.type        = GL_FLOAT;
    attribs.at(ATTRIB_WEIGHTS).offset             = offsetof(Vertex, weights);
    auto texCoordLocation                         = ATTRIB_TEXCOORD;
    for_<ATTRIB_TEXCOORD_COUNT>([&texCoordLocation, &attribs](auto i) {
        attribs.at(texCoordLocation).binding           = 0;
        attribs.at(texCoordLocation).location          = texCoordLocation;
        attribs.at(texCoordLocation).format.size       = std::remove_reference<decltype(Vertex::texCoord[i.value])>::type::length();
        attribs.at(texCoordLocation).format.normalized = false;
        attribs.at(texCoordLocation).format.type       = GL_FLOAT;
        attribs.at(texCoordLocation).offset            = offsetof(Vertex, texCoord) + i.value;
        ++texCoordLocation;
    });
    return attribs;
}
}
