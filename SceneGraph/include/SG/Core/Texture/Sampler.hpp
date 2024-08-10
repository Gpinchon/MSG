/*
 * @Author: gpinchon
 * @Date:   2021-04-29 18:56:46
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-05-01 20:10:58
 */
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Object.hpp>
#include <SG/Core/Property.hpp>

#include <glm/vec4.hpp>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
/** @brief Texture Sampler defines how the texture is sampled in shader */
class Sampler : public Inherit<Object, Sampler> {
public:
    enum class Wrap {
        Unknown = -1,
        Repeat,
        ClampToBorder,
        ClampToEdge,
        MirroredRepeat,
        MirroredClampToEdge,
        MaxValue
    };
    enum class Filter {
        Unknown = -1,
        Nearest,
        Linear,
        NearestMipmapLinear,
        NearestMipmapNearest,
        LinearMipmapLinear,
        LinearMipmapNearest,
        MaxValue
    };
    enum class CompareMode {
        Unknown = -1,
        None,
        CompareRefToTexture,
        MaxValue
    };
    enum class CompareFunc {
        Unknown = -1,
        LessEqual,
        GreaterEqual,
        Less,
        Greater,
        Equal,
        NotEqual,
        Always,
        Never,
        MaxValue
    };
    PROPERTY(Filter, MagFilter, Filter::Linear);
    PROPERTY(Filter, MinFilter, Filter::NearestMipmapLinear);
    PROPERTY(float, MinLOD, -1000);
    PROPERTY(float, MaxLOD, 1000);
    PROPERTY(float, LODBias, 0);
    PROPERTY(Wrap, WrapS, Wrap::Repeat);
    PROPERTY(Wrap, WrapT, Wrap::Repeat);
    PROPERTY(Wrap, WrapR, Wrap::Repeat);
    PROPERTY(CompareMode, CompareMode, CompareMode::None);
    PROPERTY(CompareFunc, CompareFunc, CompareFunc::Always);
    PROPERTY(float, MaxAnisotropy, 16);
    PROPERTY(glm::vec4, BorderColor, 0.f, 0.f, 0.f, 1.f);
};
}
