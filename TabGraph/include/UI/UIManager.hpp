/*
* @Author: gpinchon
* @Date:   2021-02-14 22:42:53
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-19 15:56:58
*/
#pragma once

#include <glm/fwd.hpp>

class Window;

namespace Renderer {
class Context;
class Options;
}

namespace UIManager {
void Init(const ::Window& windowHandle, Renderer::Context& renderContext);
void OnFrameBegin(uint32_t frameNbr, float delta);
void Render(const Renderer::Options& options, const glm::mat4& parentTransform, const glm::mat4& parentLastTransform);
void OnFrameEnd(uint32_t frameNbr, float delta);
};