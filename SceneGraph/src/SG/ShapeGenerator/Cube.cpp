/*
 * @Author: gpinchon
 * @Date:   2019-02-22 16:13:28
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-01-11 08:46:21
 */

#include <SG/Component/Mesh.hpp>
#include <SG/Core/Material.hpp>
#include <SG/Core/Primitive.hpp>

#include <algorithm>
#include <glm/vec3.hpp>
#include <vector>

namespace TabGraph::SG::Cube {
std::shared_ptr<Primitive> CreatePrimitive(const std::string& name, const glm::vec3& size)
{
    static std::vector<glm::vec3> cubeVertices {
        { -0.50f, -0.50f, 0.50f }, // back
        { 0.50f, -0.50f, 0.50f },
        { 0.50f, 0.50f, 0.50f },
        { -0.50f, 0.50f, 0.50f },

        { -0.50f, -0.50f, -0.50f }, // front
        { 0.50f, -0.50f, -0.50f },
        { 0.50f, 0.50f, -0.50f },
        { -0.50f, 0.50f, -0.50f },

        { -0.50f, -0.50f, -0.50f }, // left
        { -0.50f, -0.50f, 0.50f },
        { -0.50f, 0.50f, 0.50f },
        { -0.50f, 0.50f, -0.50f },

        { 0.50f, -0.50f, -0.50f }, // right
        { 0.50f, -0.50f, 0.50f },
        { 0.50f, 0.50f, 0.50f },
        { 0.50f, 0.50f, -0.50f },

        { -0.50f, 0.50f, 0.50f }, // top
        { 0.50f, 0.50f, 0.50f },
        { 0.50f, 0.50f, -0.50f },
        { -0.50f, 0.50f, -0.50f },

        { -0.50f, -0.50f, 0.50f }, // bottom
        { 0.50f, -0.50f, 0.50f },
        { 0.50f, -0.50f, -0.50f },
        { -0.50f, -0.50f, -0.50f }
    };
    static std::vector<glm::vec3> cubeNormals {
        { 0, 0, 1 },
        { 0, 0, 1 },
        { 0, 0, 1 },
        { 0, 0, 1 },

        { 0, 0, -1 },
        { 0, 0, -1 },
        { 0, 0, -1 },
        { 0, 0, -1 },

        { -1, 0, 0 },
        { -1, 0, 0 },
        { -1, 0, 0 },
        { -1, 0, 0 },

        { 1, 0, 0 },
        { 1, 0, 0 },
        { 1, 0, 0 },
        { 1, 0, 0 },

        { 0, 1, 0 },
        { 0, 1, 0 },
        { 0, 1, 0 },
        { 0, 1, 0 },

        { 0, -1, 0 },
        { 0, -1, 0 },
        { 0, -1, 0 },
        { 0, -1, 0 }
    };
    static std::vector<glm::vec2> cubeTexCoords {
        { 0, 0 },
        { 1, 0 },
        { 1, 1 },
        { 0, 1 },
        { 1, 0 },
        { 0, 0 },
        { 0, 1 },
        { 1, 1 },
        { 0, 0 },
        { 1, 0 },
        { 1, 1 },
        { 0, 1 },
        { 1, 0 },
        { 0, 0 },
        { 0, 1 },
        { 1, 1 },
        { 0, 0 },
        { 1, 0 },
        { 1, 1 },
        { 0, 1 },
        { 1, 0 },
        { 0, 0 },
        { 0, 1 },
        { 1, 1 }
    };
    static std::vector<unsigned> cubeIndices {
        0, 1, 2, 0, 2, 3, // front
        4, 6, 5, 4, 7, 6, // back
        8, 9, 10, 8, 10, 11, // left
        12, 14, 13, 12, 15, 14, // right
        16, 17, 18, 16, 18, 19, // top
        20, 22, 21, 20, 23, 22 // bottom
    };
    std::vector<glm::vec3> vertices(cubeVertices.size());
    std::transform(cubeVertices.begin(), cubeVertices.end(), vertices.begin(), [size](const auto& v) { return v * size; });
    return std::make_shared<Primitive>(vertices, cubeNormals, cubeTexCoords, cubeIndices);
}

Component::Mesh CreateMesh(const std::string& name, const glm::vec3& size)
{
    Component::Mesh m;
    m.name                                                  = name;
    m.primitives[CreatePrimitive(name + "_Geometry", size)] = std::make_shared<Material>(name + "_Material");
    return m;
}
}
