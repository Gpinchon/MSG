/*
 * @Author: gpinchon
 * @Date:   2021-07-21 21:53:09
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-07-21 22:04:50
 */

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <functional>
#include <variant>

#include <glm/gtc/matrix_transform.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Component {
class Projection {
public:
    enum class Type {
        PerspectiveInfinite,
        Perspective,
        Orthographic,
        MaxValue
    };
    Type type { Type::PerspectiveInfinite };
    struct PerspectiveInfinite {
        float fov { 45 };
        float aspectRatio { 16 / 9.f };
        float znear { 0.1f };
    };
    struct Perspective {
        float fov { 45 };
        float aspectRatio { 16 / 9.f };
        float znear { 0.1f };
        float zfar { 1000 };
    };
    struct Orthographic {
        float xmag { 50 };
        float ymag { 50 };
        float znear { 0.1f };
        float zfar { 1000 };
    };
    Projection()
        : Projection(PerspectiveInfinite())
    {
    }
    Projection(PerspectiveInfinite data);
    Projection(Perspective data);
    Projection(Orthographic data);
    template <typename T>
    inline auto& Get() const
    {
        return std::get<T>(_data);
    }
    inline auto GetMatrix() const
    {
        return _matrixFunctor(*this);
    }
    inline operator glm::mat4() const
    {
        return GetMatrix();
    }
    inline glm::mat4 operator*(const glm::mat4& other) const
    {
        return GetMatrix() * other;
    }
    inline glm::mat4 operator*(const Projection& other) const
    {
        return GetMatrix() * other.GetMatrix();
    }

private:
    std::variant<PerspectiveInfinite, Perspective, Orthographic> _data;
    std::function<glm::mat4(const Projection&)> _matrixFunctor;
};

inline Projection::Projection(PerspectiveInfinite data)
    : type(Type::PerspectiveInfinite)
    , _data(data)
    , _matrixFunctor([](const Projection& proj) {
        const auto& projData { proj.Get<PerspectiveInfinite>() };
        return glm::infinitePerspective(glm::radians(projData.fov), projData.aspectRatio, projData.znear);
    })
{
}

inline Projection::Projection(Perspective data)
    : type(Type::Perspective)
    , _data(data)
    , _matrixFunctor([](const Projection& proj) {
        const auto& projData { proj.Get<Perspective>() };
        return glm::perspective(glm::radians(projData.fov), projData.aspectRatio, projData.znear, projData.zfar);
    })
{
}

inline Projection::Projection(Orthographic data)
    : type(Type::Orthographic)
    , _data(data)
    , _matrixFunctor([](const Projection& proj) {
        const auto& projData { proj.Get<Orthographic>() };
        return glm::ortho(projData.xmag, projData.xmag, projData.ymag, projData.ymag, projData.znear, projData.zfar);
    })
{
}
}
