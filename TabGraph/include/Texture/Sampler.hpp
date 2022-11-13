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
#include <Core/Property.hpp>

#include <glm/vec4.hpp>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Textures {
/** @brief Texture Sampler defines how the texture is sampled in shader */
class Sampler {
public:
    class Impl;
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

public:
    Sampler();
    ~Sampler();

    Impl& GetImpl();

    Filter GetMagFilter() const;
    Filter GetMinFilter() const;
    float GetMinLOD() const;
    float GetMaxLOD() const;
    float GetLODBias() const;
    Wrap GetWrapS() const;
    Wrap GetWrapT() const;
    Wrap GetWrapR() const;
    CompareMode GetCompareMode() const;
    CompareFunc GetCompareFunc() const;
    float GetMaxAnisotropy() const;
    glm::vec4 GetBorderColor() const;

    void SetMagFilter(Filter value);
    void SetMinFilter(Filter value);
    void SetMinLOD(float value);
    void SetMaxLOD(float value);
    void SetLODBias(float value);
    void SetWrapS(Wrap value);
    void SetWrapT(Wrap value);
    void SetWrapR(Wrap value);
    void SetCompareMode(CompareMode value);
    void SetCompareFunc(CompareFunc value);
    void SetMaxAnisotropy(float value);
    void SetBorderColor(glm::vec4 value);
private:
    std::unique_ptr<Impl> _impl;
};

}
