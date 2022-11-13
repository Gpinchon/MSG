/*
* @Author: gpinchon
* @Date:   2021-03-16 19:54:16
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-18 01:21:05
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Object.hpp>
#include <Core/Inherit.hpp>
#include <Core/Property.hpp>
#include <SphericalHarmonics.hpp>

#include <glm/glm.hpp>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Renderer {
struct FrameRenderer;
class Framebuffer;
}
namespace Lights {
class ProbeGroup;
}
}

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Lights {
class Probe : public Core::Inherit<Core::Object, Probe> {
    PROPERTY(bool, Infinite, 0);
    PROPERTY(std::shared_ptr<Renderer::Framebuffer>, ReflectionBuffer, nullptr);
    PROPERTY(glm::vec3, Position, 0);

public:
    Probe(ProbeGroup& lightProbeGroup);

    std::vector<glm::vec3>& GetDiffuseSH() {
        return _diffuseSH;
    }
    const SphericalHarmonics& GetSphericalHarmonics() const;

    glm::vec3 GetAbsolutePosition() const;

private:
    const ProbeGroup& _lightProbeGroup;
    std::vector<glm::vec3> _diffuseSH{ 16 };
};

class ProbeGroup : public Core::Inherit<Core::Object, ProbeGroup> {
    friend Probe;
    PROPERTY(glm::vec3, Position, 0);
    READONLYPROPERTY(glm::ivec2, Resolution, 128);

public:
    ProbeGroup(size_t probesNbr, glm::ivec2 resolution = glm::ivec2(128))
        : _lightProbes(probesNbr, *this)
    {
        _SetResolution(resolution);
    }
    auto& GetLightProbes()
    {
        return _lightProbes;
    }

private:
    std::vector<Probe> _lightProbes;
    SphericalHarmonics _sphericalHarmonics{ 50 };
};
}
