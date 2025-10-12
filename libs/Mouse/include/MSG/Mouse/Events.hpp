#pragma once

#include <MSG/Events/RegisteredEvent.hpp>
#include <MSG/Mouse/Enums.hpp>
#include <MSG/Mouse/Structs.hpp>

namespace Msg {
template <typename EventName>
class EventMouse : public RegisteredEvent<EventName> {
public:
    EventMouse(
        const uint32_t& a_WindowID,
        const uint32_t& a_MouseID,
        const Mouse::Position<int32_t>& a_Position)
        : RegisteredEvent<EventName>()
        , windowID(a_WindowID)
        , mouseID(a_MouseID)
        , position(a_Position)
    {
    }
    virtual ~EventMouse() = default;
    const uint32_t windowID;
    const uint32_t mouseID;
    const Mouse::Position<int32_t> position;
};

template <typename EventName>
class EventMouseButton : public EventMouse<EventName> {
public:
    EventMouseButton(
        const uint32_t& a_WindowID,
        const uint32_t& a_MouseID,
        const Mouse::Position<int32_t>& a_Position,
        const uint8_t& a_Button,
        const uint8_t& a_Clicks)
        : EventMouse<EventName>(a_WindowID, a_MouseID, a_Position)
        , button(a_Button)
        , clicks(a_Clicks)
    {
    }
    const uint8_t button;
    const uint8_t clicks;
};

class EventMouseButtonPressed : public EventMouseButton<EventMouseButtonPressed> {
public:
    EventMouseButtonPressed(
        const uint32_t& a_WindowID,
        const uint32_t& a_MouseID,
        const Mouse::Position<int32_t>& a_Position,
        const uint8_t& a_Button,
        const uint8_t& a_Clicks)
        : EventMouseButton(a_WindowID, a_MouseID, a_Position, a_Button, a_Clicks)
    {
    }
};

class EventMouseButtonReleased : public EventMouseButton<EventMouseButtonReleased> {
public:
    EventMouseButtonReleased(
        const uint32_t& a_WindowID,
        const uint32_t& a_MouseID,
        const Mouse::Position<int32_t>& a_Position,
        const uint8_t& a_Button,
        const uint8_t& a_Clicks)
        : EventMouseButton(a_WindowID, a_MouseID, a_Position, a_Button, a_Clicks)
    {
    }
};

class EventMouseMotion : public EventMouse<EventMouseMotion> {
public:
    EventMouseMotion(
        const uint32_t& a_WindowID,
        const uint32_t& a_MouseID,
        const Mouse::Position<int32_t>& a_Position,
        const Mouse::Position<int32_t>& a_PositionRelative)
        : EventMouse(a_WindowID, a_MouseID, a_Position)
        , positionRelative(a_PositionRelative)
    {
    }
    const Mouse::Position<int32_t> positionRelative;
};

class EventMouseWheel : public EventMouse<EventMouseWheel> {
public:
    EventMouseWheel(
        const uint32_t& a_WindowID,
        const uint32_t& a_MouseID,
        const Mouse::Position<int32_t>& a_Position,
        const Mouse::Position<int32_t>& a_Amount,
        const Mouse::Position<float>& a_AmountPrecise,
        const Mouse::WeelDirection& a_Direction)
        : EventMouse(a_WindowID, a_MouseID, a_Position)
        , amount(a_Amount)
        , amountPrecise(a_AmountPrecise)
        , direction(a_Direction)
    {
    }
    const Mouse::Position<int32_t> amount;
    const Mouse::Position<float> amountPrecise;
    const Mouse::WeelDirection direction;
};
}
