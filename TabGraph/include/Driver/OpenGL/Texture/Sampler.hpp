/*
* @Author: gpinchon
* @Date:   2021-05-01 20:03:59
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-04 14:57:15
*/
#pragma once

#include "Texture/Sampler.hpp"

#include "Driver/OpenGL/ObjectHandle.hpp"

namespace TabGraph::Textures {
class Sampler::Impl {
public:
    using Handle = OpenGL::ObjectHandle;
    Impl();
    ~Impl();

    Handle GetHandle() const;

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
    Handle _handle { 0 };
};
}

namespace OpenGL {
unsigned GetEnum(TabGraph::Textures::Sampler::Filter filter);
unsigned GetEnum(TabGraph::Textures::Sampler::Wrap wrap);
unsigned GetEnum(TabGraph::Textures::Sampler::CompareMode filter);
unsigned GetEnum(TabGraph::Textures::Sampler::CompareFunc filter);
}
