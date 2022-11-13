/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-24 19:02:51
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <glm/mat4x4.hpp>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Nodes {
class Scene;
}
namespace Cameras {
class Camera;
}
namespace Shapes {
class Shape;
}
}

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Renderer {
/** @brief Handles the full rendering of a frame to the Window */
class FrameRenderer;
struct Options {
    enum class Pass {
        BeforeRender,
        DeferredGeometry,
        DeferredLighting,
        DeferredMaterial,
        ForwardTransparent,
        ForwardOpaque,
        ShadowDepth,
        AfterRender,
        MaxValue
    };
    enum class Mode {
        All,
        Opaque,
        Transparent,
        None,
        MaxValue
    };
    Options(Options::Pass pass, Options::Mode mode,
        std::shared_ptr<Cameras::Camera> camera, std::shared_ptr<Nodes::Scene> scene, std::shared_ptr<FrameRenderer> renderer,
        uint32_t frameNbr, float delta)
        : pass(pass)
        , mode(mode)
        , camera(camera)
        , scene(scene)
        , renderer(renderer)
        , frameNumber(frameNbr)
        , delta(delta) {};
    Pass pass;
    Mode mode;
    std::shared_ptr<Cameras::Camera> camera;
    std::shared_ptr<Nodes::Scene> scene;
    std::shared_ptr<FrameRenderer> renderer;
    uint32_t frameNumber;
    float delta;
};
struct ShapeState {
    glm::mat4 transform;
    std::weak_ptr<Shapes::Shape> surface;
};
};