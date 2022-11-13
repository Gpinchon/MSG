/*
* @Author: gpinchon
* @Date:   2021-03-25 11:08:35
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-12 18:39:43
*/

#include <Driver/OpenGL/Renderer/SceneRenderer.hpp>
#include <Animations/Animation.hpp>
#include <Light/Light.hpp>
#include <Light/LightProbe.hpp>
#include <Shapes/Shape.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/Light/LightRenderer.hpp>
#include <Renderer/Shape/ShapeRenderer.hpp>
#include <Nodes/Node.hpp>
#include <Nodes/Scene.hpp>
#include <Renderer/Framebuffer.hpp>
#include <Texture/Texture2D.hpp>

#include <Visitors/CullVisitor.hpp>

#include <GL/glew.h>

namespace TabGraph::Renderer {
SceneRenderer::SceneRenderer(Nodes::Scene& scene)
    : _scene(scene)
{
}

void SceneRenderer::OnFrameBegin(const Renderer::Options& options)
{
    _lightProbeDelta += options.delta;
    _fixedDelta += options.delta;
    if (_fixedDelta > 0.015) {
        _renderList.clear();
        auto fixedOptions = options;
        fixedOptions.delta = _fixedDelta;
        for (const auto& animation : _scene.GetAnimations()) {
            if (animation->GetPlaying())
                animation->Advance(_fixedDelta);
        }
        Visitors::CullVisitor cullVisitor(options, Visitors::NodeVisitor::Mode::VisitChildren);
        cullVisitor(_scene);
        for (auto& shapeState : cullVisitor.GetResult())
            _renderList.push_back(shapeState);
        //_UpdateRenderList(_scene.GetRootNode());
        for (auto& transform : _shapeLastTransform) {
            if (_shapesToKeep.count(transform.first) == 0)
                _shapeLastTransform.erase(transform.first);
        }
        for (const auto& surfaceState : _renderList) {
            auto surface{ surfaceState.surface.lock() };
            Renderer::OnFrameBegin(surface, fixedOptions);
        }
    }
    if (_lightProbeDelta > 0.032) {
        _lightProbeDelta = 0;
        for (auto& lightProbe : _lightProbeGroup.GetLightProbes()) {
            for (auto& sh : lightProbe.GetDiffuseSH())
                sh = glm::vec3(0);
        }
        bool first = true;
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        for (const auto& light : _scene.GetLights()) {
            for (auto& lightProbe : _lightProbeGroup.GetLightProbes()) {
                Renderer::UpdateLightProbe(light, options, lightProbe);
            }
            if (first) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ONE);
                glBlendEquation(GL_FUNC_ADD);
                first = false;
            }
        }
        for (auto& lightProbe : _lightProbeGroup.GetLightProbes()) {
            lightProbe.GetReflectionBuffer()->GetColorBuffer(0)->GenerateMipmap();
        }
    }
}

void SceneRenderer::Render(const Renderer::Options& options, const glm::mat4& rootMatrix)
{
    for (const auto& surfaceState : _renderList) {
        auto surface { surfaceState.surface.lock() };
        Renderer::Render(surface, options, surfaceState.transform, _shapeLastTransform.at(surfaceState.surface));
    }
}

void SceneRenderer::OnFrameEnd(const Renderer::Options& options)
{
    if (_fixedDelta > 0.015) {
        for (const auto& surfaceItr : _renderList) {
            auto surface{ surfaceItr.surface.lock() };
            Renderer::OnFrameEnd(surface, options);
        }
        _fixedDelta = 0;
    }
}

TabGraph::Lights::Probe& SceneRenderer::GetClosestLightProbe(const glm::vec3& position)
{
    TabGraph::Lights::Probe* closestLightProbe = nullptr;
    float shortestDistance = std::numeric_limits<float>::max();
    for (auto& lightProbe : _lightProbeGroup.GetLightProbes()) {
        auto distance { glm::distance(position, lightProbe.GetAbsolutePosition()) };
        if (shortestDistance > distance) {
            shortestDistance = distance;
            closestLightProbe = &lightProbe;
        }
    }
    return *closestLightProbe;
}
}
