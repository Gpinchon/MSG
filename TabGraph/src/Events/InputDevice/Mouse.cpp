/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:12:50
*/

#include <Events/InputDevice/Mouse.hpp>
#include <Events/Mouse.hpp>

#if MEDIALIBRARY == SDL2
#include <Driver/SDL2/InputDevices/Mouse.hpp>
#endif //MEDIALIBRARY == SDL2

namespace TabGraph::Events::Mouse {
InputDevice& Get()
{
    static std::unique_ptr<InputDevice> s_instance(new InputDevice);
    return *s_instance;
}

bool GetRelative()
{
    return Get().GetRelative();
}

void SetRelative(bool value)
{
    Get().SetRelative(value);
}

bool GetButtonState(Button button)
{
    return Get().GetButtonState(button);
}

glm::ivec2 GetPosition()
{
    return Get().GetPosition();
}

Signal<const Event::MouseMove&>& OnMove()
{
    return Get().onMove;
}

Signal<const Event::MouseWheel&>& OnWheel()
{
    return Get().onWheel;
}

Signal<const Event::MouseButton&>& OnButton(Button button)
{
    return Get().onButton.at(size_t(button));
}

Signal<const Event::MouseButton&>& OnButtonDown(Button button)
{
    return Get().onButtonDown.at(size_t(button));
}

Signal<const Event::MouseButton&>& OnButtonUp(Button button)
{
    return Get().onButtonUp.at(size_t(button));
}
};
