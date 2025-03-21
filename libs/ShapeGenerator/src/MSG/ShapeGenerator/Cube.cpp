#include <MSG/Material.hpp>
#include <MSG/Mesh.hpp>
#include <MSG/Mesh/Primitive.hpp>
#include <MSG/ShapeGenerator/Cube.hpp>

#include <algorithm>
#include <glm/vec3.hpp>
#include <vector>

MSG::MeshPrimitive MSG::ShapeGenerator::CreateCubePrimitive(const std::string& a_Name, const glm::vec3& size)
{
    static std::vector<glm::vec3> s_Vertices {
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
    static std::vector<glm::vec3> s_Normals {
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
    static std::vector<glm::vec2> s_TexCoords {
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
    static std::vector<unsigned> s_Indice {
        0, 1, 2, 0, 2, 3, // front
        4, 6, 5, 4, 7, 6, // back
        8, 9, 10, 8, 10, 11, // left
        12, 14, 13, 12, 15, 14, // right
        16, 17, 18, 16, 18, 19, // top
        20, 22, 21, 20, 23, 22 // bottom
    };
    std::vector<glm::vec3> vertice(s_Vertices.size());
    std::transform(s_Vertices.begin(), s_Vertices.end(), vertice.begin(), [size](const auto& v) { return v * size; });
    MeshPrimitive primitive(vertice, s_Normals, s_TexCoords, s_Indice);
    primitive.GenerateTangents();
    primitive.SetName(a_Name);
    primitive.ComputeBoundingVolume();
    return primitive;
}

MSG::Mesh MSG::ShapeGenerator::CreateCubeMesh(const std::string& a_Name, const glm::vec3& a_Size)
{
    auto primitive = std::make_shared<MeshPrimitive>(CreateCubePrimitive(a_Name + "_Primitive", a_Size));
    auto material  = std::make_shared<Material>(a_Name + "_Material");
    Mesh m(a_Name);
    m.emplace_back()[primitive] = material;
    m.ComputeBoundingVolume();
    return m;
}
