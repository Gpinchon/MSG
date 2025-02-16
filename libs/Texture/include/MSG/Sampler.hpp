#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Core/Inherit.hpp>
#include <MSG/Core/Object.hpp>
#include <MSG/Core/Property.hpp>

#include <glm/vec4.hpp>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
/** @brief Texture Sampler defines how the texture is sampled in shader */
class Sampler : public Core::Inherit<Core::Object, Sampler> {
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
    using WrapModes = std::array<Wrap, 3>;
    PROPERTY(Filter, MagFilter, Filter::Linear);
    PROPERTY(Filter, MinFilter, Filter::NearestMipmapLinear);
    PROPERTY(float, MinLOD, -1000);
    PROPERTY(float, MaxLOD, 1000);
    PROPERTY(float, LODBias, 0);
    PROPERTY(WrapModes, WrapModes, { Wrap::Repeat, Wrap::Repeat, Wrap::Repeat });
    PROPERTY(CompareMode, CompareMode, CompareMode::None);
    PROPERTY(CompareFunc, CompareFunc, CompareFunc::Always);
    PROPERTY(float, MaxAnisotropy, 16);
    PROPERTY(glm::vec4, BorderColor, 0.f, 0.f, 0.f, 1.f);

public:
    auto& GetWrapS() const { return GetWrapModes().at(0); }
    auto& GetWrapT() const { return GetWrapModes().at(1); }
    auto& GetWrapR() const { return GetWrapModes().at(2); }
    void SetWrapS(const Wrap& a_Mode) { GetWrapModes().at(0) = a_Mode; }
    void SetWrapT(const Wrap& a_Mode) { GetWrapModes().at(1) = a_Mode; }
    void SetWrapR(const Wrap& a_Mode) { GetWrapModes().at(2) = a_Mode; }
};
}
