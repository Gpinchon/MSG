/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:18
*/

#include <Shapes/Generators/Plane.hpp>
#include <Material/Standard.hpp>
#include <Shapes/Geometry.hpp>
#include <Shapes/Mesh/Mesh.hpp>

#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace TabGraph::Shapes::Generators::Plane {
std::shared_ptr<Geometry> CreateGeometry(const std::string& name, const glm::vec2& size, unsigned subdivision)
{
    glm::vec3 maxV = glm::vec3(size.x / 2.f, 0.f, size.y / 2.f);
    glm::vec3 minV = -maxV;
    glm::vec3 vn(0, 1, 0);
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoord;
    std::vector<unsigned> indices;
    //auto vertexBuffer{ std::make_shared<Buffer>(0) };
    //auto indiceBuffer{ std::make_shared<Buffer>(0) };
    for (auto x = 0u; x <= subdivision; ++x) {
        for (auto y = 0u; y <= subdivision; ++y) {
            auto uv = glm::vec2(x / float(subdivision), y / float(subdivision));
            auto v(glm::mix(minV, maxV, glm::vec3(uv.x, 0, uv.y)));
            vertices.push_back(v);
            normals.push_back(vn);
            texCoord.push_back(uv);
            if (x < subdivision && y < subdivision) {
                auto a((x + 0) + (y + 0) * (subdivision + 1));
                auto b((x + 1) + (y + 0) * (subdivision + 1));
                auto c((x + 0) + (y + 1) * (subdivision + 1));
                auto d((x + 1) + (y + 1) * (subdivision + 1));
                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(c);
                indices.push_back(b);
                indices.push_back(d);
                indices.push_back(c);
            }
        }
    }
    return std::make_shared<Geometry>(vertices, normals, texCoord, indices);
}
std::shared_ptr<Mesh> CreateMesh(const std::string& name, const glm::vec2& size, unsigned subdivision)
{
    auto m = std::make_shared<Mesh>(name);
    
    m->AddGeometry(
        CreateGeometry(name + "Geometry", size, subdivision),
        std::make_shared<Material::Standard>(m->GetName() + "Material")
    );
    return (m);
}
}