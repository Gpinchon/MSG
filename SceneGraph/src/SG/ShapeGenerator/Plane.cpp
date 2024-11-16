#include <SG/Core/Material.hpp>
#include <SG/ShapeGenerator/Plane.hpp>

namespace TabGraph::SG {
Primitive CreatePlanePrimitive(const std::string& a_Name, const glm::vec3& a_Size, unsigned a_Subdivision)
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoord;
    std::vector<unsigned> indices;
    glm::vec3 maxV = glm::vec3(a_Size.x / 2.f, 0.f, a_Size.y / 2.f);
    glm::vec3 minV = -maxV;
    glm::vec3 vn(0, 1, 0);
    for (auto x = 0u; x <= a_Subdivision; ++x) {
        for (auto y = 0u; y <= a_Subdivision; ++y) {
            auto uv = glm::vec2(x / float(a_Subdivision), y / float(a_Subdivision));
            auto v(glm::mix(minV, maxV, glm::vec3(uv.x, 0, uv.y)));
            vertices.push_back(v);
            normals.push_back(vn);
            texCoord.push_back(uv);
            if (x < a_Subdivision && y < a_Subdivision) {
                auto a((x + 0) + (y + 0) * (a_Subdivision + 1));
                auto b((x + 1) + (y + 0) * (a_Subdivision + 1));
                auto c((x + 0) + (y + 1) * (a_Subdivision + 1));
                auto d((x + 1) + (y + 1) * (a_Subdivision + 1));
                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(c);
                indices.push_back(b);
                indices.push_back(d);
                indices.push_back(c);
            }
        }
    }
    Primitive primitive(vertices, normals, texCoord, indices);
    primitive.GenerateTangents();
    primitive.SetName(a_Name);
    primitive.ComputeBoundingVolume();
    return primitive;
}

Component::Mesh CreatePlaneMesh(const std::string& a_Name, const glm::vec3& a_Size, unsigned a_Subdivision)
{
    auto primitive = std::make_shared<Primitive>(CreatePlanePrimitive(a_Name + "_Primitive", a_Size, a_Subdivision));
    auto material  = std::make_shared<Material>(a_Name + "_Material");
    Component::Mesh m;
    m.name                  = a_Name;
    m.primitives[primitive] = material;
    m.ComputeBoundingVolume();
    return m;
}
}
