/*
* @Author: gpinchon
* @Date:   2021-04-29 18:57:43
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-01 23:56:53
*/
#include <Texture/Sampler.hpp>
#if RENDERINGAPI == OpenGL
#include <Driver/OpenGL/Texture/Sampler.hpp>
#endif

#include <array>

namespace TabGraph::Textures {
Sampler::Sampler()
    : _impl(new Sampler::Impl)
{
}

Sampler::~Sampler()
{
}

Sampler::Impl& Sampler::GetImpl()
{
    return *_impl.get();
}

Sampler::Filter Sampler::GetMagFilter() const
{
    return _impl->GetMagFilter();
}

Sampler::Filter Sampler::GetMinFilter() const
{
    return _impl->GetMinFilter();
}

float Sampler::GetMinLOD() const
{
    return _impl->GetMinLOD();
}

float Sampler::GetMaxLOD() const
{
    return _impl->GetMaxLOD();
}

float Sampler::GetLODBias() const
{
    return _impl->GetLODBias();
}

Sampler::Wrap Sampler::GetWrapS() const
{
    return _impl->GetWrapS();
}

Sampler::Wrap Sampler::GetWrapT() const
{
    return _impl->GetWrapT();
}

Sampler::Wrap Sampler::GetWrapR() const
{
    return _impl->GetWrapR();
}

Sampler::CompareMode Sampler::GetCompareMode() const
{
    return _impl->GetCompareMode();
}

Sampler::CompareFunc Sampler::GetCompareFunc() const
{
    return _impl->GetCompareFunc();
}

float Sampler::GetMaxAnisotropy() const
{
    return _impl->GetMaxAnisotropy();
}

glm::vec4 Sampler::GetBorderColor() const
{
    return _impl->GetBorderColor();
}

void Sampler::SetMagFilter(Filter value)
{
    _impl->SetMagFilter(value);
}

void Sampler::SetMinFilter(Filter value)
{
    _impl->SetMinFilter(value);
}

void Sampler::SetMinLOD(float value)
{
    _impl->SetMinLOD(value);
}

void Sampler::SetMaxLOD(float value)
{
    _impl->SetMaxLOD(value);
}

void Sampler::SetLODBias(float value)
{
    _impl->SetLODBias(value);
}

void Sampler::SetWrapS(Wrap value)
{
    _impl->SetWrapS(value);
}

void Sampler::SetWrapT(Wrap value)
{
    _impl->SetWrapT(value);
}

void Sampler::SetWrapR(Wrap value)
{
    _impl->SetWrapR(value);
}

void Sampler::SetCompareMode(CompareMode value)
{
    _impl->SetCompareMode(value);
}

void Sampler::SetCompareFunc(CompareFunc value)
{
    _impl->SetCompareFunc(value);
}

void Sampler::SetMaxAnisotropy(float value)
{
    _impl->SetMaxAnisotropy(value);
}

void Sampler::SetBorderColor(glm::vec4 value)
{
    _impl->SetBorderColor(value);
}

}
