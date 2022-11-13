#pragma once

#include <Events/Event.hpp>
#include <Events/Mouse.hpp>
#include <Events/Signal.hpp>

#include <glm/vec2.hpp>

namespace TabGraph::Events::Mouse {
bool GetRelative();
void SetRelative(bool relative);
bool GetButtonState(Button button);
glm::ivec2 GetPosition();
Signal<const Event::MouseMove&>& OnMove();
Signal<const Event::MouseWheel&>& OnWheel();
Signal<const Event::MouseButton&>& OnButton(Button button);
Signal<const Event::MouseButton&>& OnButtonDown(Button button);
Signal<const Event::MouseButton&>& OnButtonUp(Button button);
}
