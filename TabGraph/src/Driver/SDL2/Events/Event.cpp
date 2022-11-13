/*
* @Author: gpinchon
* @Date:   2021-05-20 12:13:34
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-25 18:59:28
*/

#include <Driver/SDL2/Events/Event.hpp>
#include <Driver/SDL2/InputDevices/GameController.hpp>
#include <Driver/SDL2/InputDevices/Keyboard.hpp>
#include <Driver/SDL2/InputDevices/Mouse.hpp>
#include <Driver/SDL2/Window.hpp>

#include <SDL_events.h>

namespace TabGraph::SDL2 {
const static std::map<SDL_EventType, TabGraph::Events::Event::Type> s_eventTypeLUT {
    { SDL_FIRSTEVENT,TabGraph::Events::Event::Type::Unknown },
    { SDL_QUIT,TabGraph::Events::Event::Type::Quit },
    { SDL_APP_TERMINATING,TabGraph::Events::Event::Type::AppTerminating },
    { SDL_APP_LOWMEMORY,TabGraph::Events::Event::Type::AppLowMemory },
    { SDL_APP_WILLENTERBACKGROUND,TabGraph::Events::Event::Type::AppWillenterBackground },
    { SDL_APP_DIDENTERBACKGROUND,TabGraph::Events::Event::Type::AppDidenterBackground },
    { SDL_APP_WILLENTERFOREGROUND,TabGraph::Events::Event::Type::AppWillenterForeground },
    { SDL_APP_DIDENTERFOREGROUND,TabGraph::Events::Event::Type::AppDidenterForeground },
    { SDL_LOCALECHANGED,TabGraph::Events::Event::Type::LocaleChanged },
    { SDL_WINDOWEVENT,TabGraph::Events::Event::Type::WindowEvent },
    { SDL_KEYDOWN,TabGraph::Events::Event::Type::KeyDown },
    { SDL_KEYUP,TabGraph::Events::Event::Type::KeyUp },
    { SDL_TEXTEDITING,TabGraph::Events::Event::Type::TextEditing },
    { SDL_TEXTINPUT,TabGraph::Events::Event::Type::TextInput },
    { SDL_KEYMAPCHANGED,TabGraph::Events::Event::Type::KeymapChanged },
    { SDL_MOUSEMOTION,TabGraph::Events::Event::Type::MouseMotion },
    { SDL_MOUSEBUTTONDOWN,TabGraph::Events::Event::Type::MouseButtonDown },
    { SDL_MOUSEBUTTONUP,TabGraph::Events::Event::Type::MouseButtonUp },
    { SDL_MOUSEWHEEL,TabGraph::Events::Event::Type::MouseWheel },
    { SDL_JOYAXISMOTION,TabGraph::Events::Event::Type::JoyAxisMotion },
    { SDL_JOYBALLMOTION,TabGraph::Events::Event::Type::JoyBallMotion },
    { SDL_JOYHATMOTION,TabGraph::Events::Event::Type::JoyHatMotion },
    { SDL_JOYBUTTONDOWN,TabGraph::Events::Event::Type::JoyButtonDown },
    { SDL_JOYBUTTONUP,TabGraph::Events::Event::Type::JoyButtonUp },
    { SDL_JOYDEVICEADDED,TabGraph::Events::Event::Type::JoyDeviceAdded },
    { SDL_JOYDEVICEREMOVED,TabGraph::Events::Event::Type::JoyDeviceRemoved },
    { SDL_CONTROLLERAXISMOTION,TabGraph::Events::Event::Type::ControllerAxisMotion },
    { SDL_CONTROLLERBUTTONDOWN,TabGraph::Events::Event::Type::ControllerButtonDown },
    { SDL_CONTROLLERBUTTONUP,TabGraph::Events::Event::Type::ControllerButtonUp },
    { SDL_CONTROLLERDEVICEADDED,TabGraph::Events::Event::Type::ControllerDeviceAdded },
    { SDL_CONTROLLERDEVICEREMOVED,TabGraph::Events::Event::Type::ControllerDeviceRemoved },
    { SDL_CONTROLLERDEVICEREMAPPED,TabGraph::Events::Event::Type::ControllerDeviceRemapped },
    { SDL_CONTROLLERTOUCHPADDOWN,TabGraph::Events::Event::Type::ControllerTouchpadDown },
    { SDL_CONTROLLERTOUCHPADMOTION,TabGraph::Events::Event::Type::ControllerTouchpadMotion },
    { SDL_CONTROLLERTOUCHPADUP,TabGraph::Events::Event::Type::ControllerTouchpadUp },
    { SDL_CONTROLLERSENSORUPDATE,TabGraph::Events::Event::Type::ControllerSensorUpdate },
    { SDL_FINGERDOWN,TabGraph::Events::Event::Type::FingerDown },
    { SDL_FINGERUP,TabGraph::Events::Event::Type::FingerUp },
    { SDL_FINGERMOTION,TabGraph::Events::Event::Type::FingerMotion },
    { SDL_DOLLARGESTURE,TabGraph::Events::Event::Type::DollarGesture },
    { SDL_DOLLARRECORD,TabGraph::Events::Event::Type::DollarRecord },
    { SDL_MULTIGESTURE,TabGraph::Events::Event::Type::MultiGesture },
    { SDL_CLIPBOARDUPDATE,TabGraph::Events::Event::Type::ClipboardUpdate },
    { SDL_DROPFILE,TabGraph::Events::Event::Type::DropFile },
    { SDL_DROPTEXT,TabGraph::Events::Event::Type::DropText },
    { SDL_DROPBEGIN,TabGraph::Events::Event::Type::DropBegin },
    { SDL_DROPCOMPLETE,TabGraph::Events::Event::Type::DropComplete },
    { SDL_AUDIODEVICEADDED,TabGraph::Events::Event::Type::AudioDeviceAdded },
    { SDL_AUDIODEVICEREMOVED,TabGraph::Events::Event::Type::AudioDeviceremoved },
    { SDL_SENSORUPDATE,TabGraph::Events::Event::Type::SensorUpdate },
    { SDL_USEREVENT,TabGraph::Events::Event::Type::User },
    { SDL_LASTEVENT,TabGraph::Events::Event::Type::MaxValue }
};

TabGraph::Events::Event::Type GetEventType(uint32_t type)
{
    return s_eventTypeLUT.at(SDL_EventType(type));
};

TabGraph::Events::Event CreateEvent(const SDL_Event* SDLevent)
{
    TabGraph::Events::Event event;
    event.type = GetEventType(SDLevent->type);
    switch (SDLevent->type) {
    case SDL_QUIT:
        break;
    case SDL_APP_TERMINATING:
        break;
    case SDL_APP_LOWMEMORY:
        break;
    case SDL_APP_WILLENTERBACKGROUND:
        break;
    case SDL_APP_DIDENTERBACKGROUND:
        break;
    case SDL_APP_WILLENTERFOREGROUND:
        break;
    case SDL_APP_DIDENTERFOREGROUND:
        break;
    case SDL_LOCALECHANGED:
        break;
    case SDL_DISPLAYEVENT:
        break;
    case SDL_WINDOWEVENT:
        event.data = SDL2::Window::CreateEventData(SDLevent->window);
        break;
    case SDL_SYSWMEVENT:
        break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        event.data = SDL2::Keyboard::CreateEventData(SDLevent->key);
        break;
    case SDL_TEXTEDITING:
        event.data = SDL2::Keyboard::CreateEventData(SDLevent->edit);
        break;
    case SDL_TEXTINPUT:
        event.data = SDL2::Keyboard::CreateEventData(SDLevent->text);
        break;
    case SDL_KEYMAPCHANGED:
        break;
    case SDL_MOUSEMOTION:
        event.data = SDL2::Mouse::CreateEventData(SDLevent->motion);
        break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        event.data = SDL2::Mouse::CreateEventData(SDLevent->button);
        break;
    case SDL_MOUSEWHEEL:
        event.data = SDL2::Mouse::CreateEventData(SDLevent->wheel);
        break;
    case SDL_JOYAXISMOTION:
        break;
    case SDL_JOYBALLMOTION:
        break;
    case SDL_JOYHATMOTION:
        break;
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
        break;
    case SDL_JOYDEVICEADDED:
        break;
    case SDL_JOYDEVICEREMOVED:
        break;
    case SDL_CONTROLLERAXISMOTION:
        event.data = SDL2::GameController::CreateEventData(SDLevent->caxis);
        break;
    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_CONTROLLERBUTTONUP:
        event.data = SDL2::GameController::CreateEventData(SDLevent->cbutton);
        break;
    case SDL_CONTROLLERDEVICEADDED:
    case SDL_CONTROLLERDEVICEREMOVED:
        event.data = SDL2::GameController::CreateEventData(SDLevent->cdevice);
        break;
    case SDL_CONTROLLERDEVICEREMAPPED:
        break;
    case SDL_CONTROLLERTOUCHPADDOWN:
        break;
    case SDL_CONTROLLERTOUCHPADMOTION:
        break;
    case SDL_CONTROLLERTOUCHPADUP:
        break;
    case SDL_CONTROLLERSENSORUPDATE:
        break;
    case SDL_FINGERDOWN:
        break;
    case SDL_FINGERUP:
        break;
    case SDL_FINGERMOTION:
        break;
    case SDL_DOLLARGESTURE:
        break;
    case SDL_DOLLARRECORD:
        break;
    case SDL_MULTIGESTURE:
        break;
    case SDL_CLIPBOARDUPDATE:
        break;
    case SDL_DROPFILE:
        break;
    case SDL_DROPTEXT:
        break;
    case SDL_DROPBEGIN:
        break;
    case SDL_DROPCOMPLETE:
        break;
    case SDL_AUDIODEVICEADDED:
        break;
    case SDL_AUDIODEVICEREMOVED:
        break;
    case SDL_SENSORUPDATE:
        break;
    case SDL_RENDER_TARGETS_RESET:
        break;
    case SDL_RENDER_DEVICE_RESET:
        break;
    default:
        throw std::runtime_error("UnknownTabGraph::Events::Event::Type");
    }
    return event;
};
};
