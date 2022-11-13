/*
* @Author: gpinchon
* @Date:   2020-05-01 21:59:49
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-17 13:53:16
*/

#include <Physics/BoundingMesh.hpp>
#include <Buffer/Accessor.hpp>
#include <Shapes/Geometry.hpp>
#include <Shapes/Mesh/Mesh.hpp>

#include <glm/gtc/matrix_inverse.hpp>

namespace TabGraph::Physics {
BoundingMesh::BoundingMesh(const std::shared_ptr<Shapes::Mesh>& geometry)
    : Inherit(BoundingElement::Type::Geometry)
{
    SetMesh(geometry);
}

std::set<glm::vec3, compareVec> BoundingMesh::GetSATAxis(const glm::mat4& transform) const
{
    auto normalMatrix(glm::inverseTranspose(transform));
    auto mesh = GetMesh();
    std::set<glm::vec3, compareVec> axis;
    for (const auto& pair : mesh->GetGeometries()) {
        auto geometry(pair.first);
        if (!geometry->GetIndices().empty()) {
            //GOOD NEWS EVERYONE ! We've got indices !
            for (const auto& index : static_cast<Buffer::TypedAccessor<unsigned>>(geometry->GetIndices())) {
                auto &v(geometry->GetNormals().at<glm::vec3>(index));
                glm::vec3 transformedNormal(normalMatrix * glm::vec4(v, 1.f));
                axis.insert(transformedNormal);
            }
            continue;
        }
        for (const auto& v : static_cast<Buffer::TypedAccessor<glm::vec3>>(geometry->GetNormals())) {
            glm::vec3 transformedNormal(normalMatrix * glm::vec4(v, 0.f));
            axis.insert(transformedNormal);
        }
    }
    return axis;
}

BoundingElement::ProjectionInterval BoundingMesh::Project(const glm::vec3& axis, const glm::mat4& transform) const
{
    auto vec = axis;
    if (length(vec) < 0.0001) {
        vec = glm::vec3(1, 0, 0);
    } else
        vec = normalize(vec);
    auto mesh = GetMesh();
    auto minDot = std::numeric_limits<float>::max();
    auto maxDot = std::numeric_limits<float>::lowest();
    glm::vec3 maxV(std::numeric_limits<float>::infinity());
    glm::vec3 minV(std::numeric_limits<float>::infinity());
    for (const auto& pair : mesh->GetGeometries()) {
        auto geometry{pair.first};
        for (const auto& v : static_cast<Buffer::TypedAccessor<glm::vec3>>(geometry->GetPositions())) {
            glm::vec3 transformedVertex(transform * glm::vec4(v, 1.f));
            auto dotProd(glm::dot(transformedVertex, vec));
            if (dotProd < minDot) {
                minDot = dotProd;
                minV = transformedVertex;
            }
            if (dotProd > maxDot) {
                maxDot = dotProd;
                maxV = transformedVertex;
            }
        }
    }
    return ProjectionInterval(vec, minV, maxV);
}

glm::mat3 BoundingMesh::LocalInertiaTensor(const float& mass) const
{
    auto maxI = glm::vec3(1.f);
    auto minI = glm::vec3(-1.f);
    auto w = fabs(maxI.x - minI.x);
    auto w2 = w * w;
    auto h = fabs(maxI.y - minI.y);
    auto h2 = h * h;
    auto d = fabs(maxI.z - minI.z);
    auto d2 = d * d;
    auto m = 1 / 12.f * mass;
    return {
        m * (h2 + d2), 0, 0,
        0, m * (w2 + d2), 0,
        0, 0, m * (w2 + h2)
    };
}

static inline auto IsInFront(glm::vec3 axis, glm::vec3 point)
{
    return dot(axis, point) > 0;
}

static inline auto IntersectionPoint(glm::vec3 axis, glm::vec3 a, glm::vec3 b)
{
    auto dir(normalize(a - b));
    float denom(dot(axis, dir));
    auto p(-a);
    auto t(dot(p, axis) / denom);
    return a + t * dir;
}

std::vector<glm::vec3> BoundingMesh::Clip(glm::vec3 axis, const glm::mat4& transform) const
{
    auto mesh = GetMesh();
    std::vector<glm::vec3> output;
    for (const auto& pair : mesh->GetGeometries()) {
        auto geometry{ pair.first };
        if (!geometry->GetIndices().empty()) {
            auto startingPoint(geometry->GetPositions().at<glm::vec3>(geometry->GetIndices().GetSize() - 1));
            for (const auto& index : static_cast<Buffer::TypedAccessor<unsigned>>(geometry->GetIndices())) {
                auto v(geometry->GetPositions().at<glm::vec3>(index));
                glm::vec3 endPoint(transform * glm::vec4(v, 1.f));
                if (IsInFront(axis, endPoint)) {
                    if (!IsInFront(axis, startingPoint))
                        output.push_back(IntersectionPoint(axis, startingPoint, endPoint));
                    output.push_back(endPoint);
                } else if (IsInFront(axis, startingPoint))
                    output.push_back(IntersectionPoint(axis, startingPoint, endPoint));
                startingPoint = endPoint;
            }
            continue;
        }
        auto startingPoint(geometry->GetPositions().at<glm::vec3>(geometry->GetPositions().GetSize() - 1));
        for (const auto& v : static_cast<Buffer::TypedAccessor<glm::vec3>>(geometry->GetPositions())) {
            glm::vec3 endPoint(transform * glm::vec4(v, 1.f));
            if (IsInFront(axis, endPoint)) {
                if (!IsInFront(axis, startingPoint))
                    output.push_back(IntersectionPoint(axis, startingPoint, endPoint));
                output.push_back(endPoint);
            } else if (IsInFront(axis, startingPoint))
                output.push_back(IntersectionPoint(axis, startingPoint, endPoint));
            startingPoint = endPoint;
        }
    }
    return output;
}
}