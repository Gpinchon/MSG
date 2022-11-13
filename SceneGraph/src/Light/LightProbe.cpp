/*
* @Author: gpinchon
* @Date:   2021-03-18 01:33:17
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:02:23
*/

#include <Light/LightProbe.hpp>
#include <Renderer/Framebuffer.hpp>
#include <Texture/TextureCubemap.hpp>
#include <Texture/Sampler.hpp>

namespace TabGraph::Lights {
Probe::Probe(ProbeGroup& lightProbeGroup)
    : _lightProbeGroup(lightProbeGroup)
{
    SetReflectionBuffer(std::make_shared<Renderer::Framebuffer>(lightProbeGroup.GetResolution()));
    auto cubemap {std::make_shared<Textures::TextureCubemap>(lightProbeGroup.GetResolution(), Pixel::SizedFormat::Float16_RGB) };
    cubemap->GetSampler()->SetMinFilter(Textures::Sampler::Filter::LinearMipmapLinear);
    GetReflectionBuffer()->AddColorBuffer(cubemap);
}

const SphericalHarmonics& Probe::GetSphericalHarmonics() const
{
    return _lightProbeGroup._sphericalHarmonics;
}

glm::vec3 Probe::GetAbsolutePosition() const
{
    return _lightProbeGroup.GetPosition() + GetPosition();
}
}
