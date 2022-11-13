/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:12:49
*/

#include <Events/InputDevice/GameController.hpp>

#if MEDIALIBRARY == SDL2
#include <Driver/SDL2/InputDevices/GameController.hpp>
#endif //MEDIALIBRARY == SDL2

#include <iostream> // for basic_ostream::operator<<, operator<<, endl
#include <map>

namespace TabGraph::Events::GameController {
static inline InputDevice& Get()
{
    static std::unique_ptr<InputDevice> s_instance(new InputDevice);
    return *s_instance;
}
Signal<const Event::GameControllerAxis&>& OnAxis(uint8_t index, const GameController::Axis& axis)
{
    return Get().OnAxis(index, axis);
}
Signal<const Event::GameControllerButton&>& OnButton(uint8_t index, const GameController::Button& button)
{
    return Get().OnButton(index, button);
}
Signal<const Event::GameControllerButton&>& OnButtonUp(uint8_t index, const GameController::Button& button)
{
    return Get().OnButtonUp(index, button);
}
Signal<const Event::GameControllerButton&>& OnButtonDown(uint8_t index, const GameController::Button& button)
{
    return Get().OnButtonDown(index, button);
}
Signal<const Event::GameControllerDevice&>& OnConnection(uint8_t index)
{
    return Get().OnConnection(index);
}
Signal<const Event::GameControllerDevice&>& OnDisconnection(uint8_t index)
{
    return Get().OnDisconnection(index);
}
bool Rumble(uint8_t index, float lowFrequency, float highFrequency, int duration)
{
    return Get().Rumble(index, lowFrequency, highFrequency, duration);
}
bool RumbleTriggers(uint8_t index, float left_rumble, float right_rumble, uint32_t duration)
{
    return Get().RumbleTriggers(index, left_rumble, right_rumble, duration);
}
bool SetLED(uint8_t index, const glm::vec3& color)
{
    return Get().SetLED(index, color);
}
float GetAxis(uint8_t index, GameController::Axis axis)
{
    return Get().GetAxis(index, axis);
}
bool GetButton(uint8_t index, GameController::Button button)
{
    return Get().GetButton(index, button);
}
};
