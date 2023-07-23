#pragma once

#include <OCRA/OCRA.hpp>

#include <glm/glm.hpp>

namespace TabGraph::Renderer {
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec4 tangent;
    glm::vec2 texCoord_0;
    glm::vec2 texCoord_1;
    glm::vec2 texCoord_2;
    glm::vec2 texCoord_3;
    glm::vec3 color;
    glm::u16vec4 joints;
    glm::vec4 weights;
    uint32_t _padding[5];
    inline static auto GetBindingDescriptions()
    {
        std::vector<OCRA::VertexBindingDescription> bindings(1);
        bindings.at(0).binding = 0;
        bindings.at(0).stride = sizeof(Vertex);
        bindings.at(0).inputRate = OCRA::VertexInputRate::Vertex;
        return bindings;
    }
    inline static auto GetAttributeDescription()
    {
        uint8_t location = 0;
        std::vector<OCRA::VertexAttributeDescription> attribs(10);
        attribs.at(location).binding = 0;
        attribs.at(location).location = location;
        attribs.at(location).format.size = decltype(position)::length();
        attribs.at(location).format.normalized = false;
        attribs.at(location).format.type = OCRA::VertexType::Float32;
        attribs.at(location).offset = offsetof(Vertex, position);
        ++location;
        attribs.at(location).binding = 0;
        attribs.at(location).location = location;
        attribs.at(location).format.size = decltype(normal)::length();
        attribs.at(location).format.normalized = true;
        attribs.at(location).format.type = OCRA::VertexType::Float32;
        attribs.at(location).offset = offsetof(Vertex, normal);
        ++location;
        attribs.at(location).binding = 0;
        attribs.at(location).location = location;
        attribs.at(location).format.size = decltype(tangent)::length();
        attribs.at(location).format.normalized = false;
        attribs.at(location).format.type = OCRA::VertexType::Float32;
        attribs.at(location).offset = offsetof(Vertex, tangent);
        ++location;
        attribs.at(location).binding = 0;
        attribs.at(location).location = location;
        attribs.at(location).format.size = decltype(texCoord_0)::length();
        attribs.at(location).format.normalized = false;
        attribs.at(location).format.type = OCRA::VertexType::Float32;
        attribs.at(location).offset = offsetof(Vertex, texCoord_0);
        ++location;
        attribs.at(location).binding = 0;
        attribs.at(location).location = location;
        attribs.at(location).format.size = decltype(texCoord_1)::length();
        attribs.at(location).format.normalized = false;
        attribs.at(location).format.type = OCRA::VertexType::Float32;
        attribs.at(location).offset = offsetof(Vertex, texCoord_1);
        ++location;
        attribs.at(location).binding = 0;
        attribs.at(location).location = location;
        attribs.at(location).format.size = decltype(texCoord_2)::length();
        attribs.at(location).format.normalized = false;
        attribs.at(location).format.type = OCRA::VertexType::Float32;
        attribs.at(location).offset = offsetof(Vertex, texCoord_2);
        ++location;
        attribs.at(location).binding = 0;
        attribs.at(location).location = location;
        attribs.at(location).format.size = decltype(texCoord_3)::length();
        attribs.at(location).format.normalized = false;
        attribs.at(location).format.type = OCRA::VertexType::Float32;
        attribs.at(location).offset = offsetof(Vertex, texCoord_3);
        ++location;
        attribs.at(location).binding = 0;
        attribs.at(location).location = location;
        attribs.at(location).format.size = decltype(color)::length();
        attribs.at(location).format.normalized = false;
        attribs.at(location).format.type = OCRA::VertexType::Float32;
        attribs.at(location).offset = offsetof(Vertex, color);
        ++location;
        attribs.at(location).binding = 0;
        attribs.at(location).location = location;
        attribs.at(location).format.size = decltype(joints)::length();
        attribs.at(location).format.normalized = false;
        attribs.at(location).format.type = OCRA::VertexType::Uint16;
        attribs.at(location).offset = offsetof(Vertex, joints);
        ++location;
        attribs.at(location).binding = 0;
        attribs.at(location).location = location;
        attribs.at(location).format.size = decltype(weights)::length();
        attribs.at(location).format.normalized = false;
        attribs.at(location).format.type = OCRA::VertexType::Float32;
        attribs.at(location).offset = offsetof(Vertex, weights);
        ++location;
        return attribs;
    }
};
}