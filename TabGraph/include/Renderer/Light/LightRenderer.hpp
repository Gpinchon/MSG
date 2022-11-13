/*
* @Author: gpinchon
* @Date:   2021-04-10 15:27:18
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-12 22:01:09
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Lights {
class Light;
class Probe;
}
namespace Renderer {
struct Options;
}
}


////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Renderer {
void Render(std::shared_ptr<TabGraph::Lights::Light>, const Renderer::Options&);
void UpdateLightProbe(std::shared_ptr<TabGraph::Lights::Light>, const Renderer::Options&, TabGraph::Lights::Probe&);
class LightRenderer {
public:
    LightRenderer(TabGraph::Lights::Light&);
    virtual void Render(const Renderer::Options&) = 0;
    virtual void UpdateLightProbe(const Renderer::Options&, TabGraph::Lights::Probe&) = 0;
    void FlagDirty();

protected:
    TabGraph::Lights::Light& _light;
    bool _dirty{ true };
};
};
