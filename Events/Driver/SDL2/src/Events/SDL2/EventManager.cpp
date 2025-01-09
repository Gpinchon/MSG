#include <Events/EventManager.hpp>
#include <Events/SDL2/EventManager.hpp>
#include <SDL2/SDL.h>

#include <cassert>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace std {
template <typename T>
struct hash;
template <>
struct hash<TabGraph::EventTypeID> {
    size_t operator()(TabGraph::EventTypeID const& a_Value) const
    {
        return std::hash<TabGraph::EventTypeID::type> {}(TabGraph::EventTypeID::type(a_Value));
    }
};
template <>
struct hash<TabGraph::EventBindingID> {
    size_t operator()(TabGraph::EventBindingID const& a_Value) const
    {
        return std::hash<TabGraph::EventBindingID::type> {}(TabGraph::EventBindingID::type(a_Value));
    }
};
}

namespace TabGraph::Events {
struct CallbackStorage {
    EventCallback callback;
    EventTypeID eventTypeID;
    std::any userData;
};

struct EventListenerStorage {
    EventListenerCallback callback;
    std::any userData;
};

using Callbacks       = std::unordered_map<EventBindingID, CallbackStorage>;
using TypeToBindings  = std::unordered_map<EventTypeID, std::vector<EventBindingID>>;
using BindingIDs      = std::queue<EventBindingID>;
using Events          = std::queue<std::unique_ptr<Event>>;
using RegisteredTypes = std::unordered_set<EventTypeID>;
using EventListeners  = std::unordered_map<SDL_EventType, EventListenerStorage>;

class EventManager {
public:
    EventManager()
    {
        SDL_Init(SDL_INIT_EVENTS);
    }
    void Update()
    {
        std::scoped_lock lock(_mutex);
        SDL_PumpEvents();
        auto eventNbr = SDL_PeepEvents(nullptr, 0, SDL_PEEKEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT);
        std::vector<SDL_Event> events(eventNbr);
        SDL_PeepEvents(events.data(), events.size(), SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT);
        for (auto& event : events) {
            auto eventListenerItr = _eventListeners.find(SDL_EventType(event.type));
            if (eventListenerItr != _eventListeners.end())
                eventListenerItr->second.callback(event, eventListenerItr->second.userData);
        }
    }
    EventBindingID BindCallback(const EventTypeID& a_TypeID, const EventCallback& a_Callback, std::any a_UserData = {})
    {
        std::scoped_lock lock(_mutex);
        auto bindingID = _GetFreeBindingID();
        CallbackStorage storage;
        storage.eventTypeID = a_TypeID;
        storage.callback    = a_Callback;
        storage.userData    = a_UserData;
        _callbacks.insert({ bindingID, storage });
        _typeToBinding[a_TypeID].emplace_back(bindingID);
        return bindingID;
    }
    void UnbindCallback(const EventBindingID& a_BindingID)
    {
        std::scoped_lock lock(_mutex);
        _RestoreBindingID(a_BindingID);
        _typeToBinding.erase(_callbacks.at(a_BindingID).eventTypeID);
        _callbacks.erase(a_BindingID);
    }
    EventTypeID RegisterType(const EventTypeID& a_Hint)
    {
        std::scoped_lock lock(_mutex);
        auto newTypeID = a_Hint != EventTypeNone ? a_Hint : EventTypeID(_currentTypeID);
        while (!_registeredTypes.insert(newTypeID).second) // try until we can insert a new Event ID
            newTypeID = EventTypeID(_currentTypeID++);
        return newTypeID;
    }
    void Push(std::unique_ptr<Event>& a_Event)
    {
        std::scoped_lock lock(_mutex);
        _eventQueue.push(std::move(a_Event));
    }
    void PushNoLock(std::unique_ptr<Event>& a_Event) { _eventQueue.push(std::move(a_Event)); }
    std::unique_ptr<Event> Poll()
    {
        std::scoped_lock lock(_mutex);
        return _Poll();
    }
    void Consume()
    {
        std::scoped_lock lock(_mutex);
        for (auto event = _Poll(); event != nullptr; event = Poll()) {
            auto bindingIDsItr = _typeToBinding.find(event->typeID);
            if (bindingIDsItr == _typeToBinding.end())
                continue;
            for (auto& bindingID : bindingIDsItr->second) {
                auto& callback = _callbacks.at(bindingID);
                callback.callback(*event, bindingID, callback.userData);
            }
        }
    }
    void SetEventListener(const SDL_EventType& a_EventType, const EventListenerCallback& a_Listener, std::any a_UserData)
    {
        _eventListeners[a_EventType] = {
            .callback = a_Listener,
            .userData = a_UserData
        };
    }
    void RemoveEventListener(const SDL_EventType& a_EventType)
    {
        assert(_eventListeners.contains(a_EventType));
        _eventListeners.erase(a_EventType);
    }

private:
    std::unique_ptr<Event> _Poll()
    {
        if (_eventQueue.empty())
            return {};
        auto event = std::move(_eventQueue.front());
        _eventQueue.pop();
        return event;
    }
    EventBindingID _GetFreeBindingID()
    {
        EventBindingID newBindingID(_currentBindingID);
        if (!_freeBindingIDs.empty()) {
            newBindingID = _freeBindingIDs.front();
            _freeBindingIDs.pop();
        } else
            _currentBindingID++;
        return newBindingID;
    }
    void _RestoreBindingID(const EventBindingID& a_BindingID)
    {
        _freeBindingIDs.push(a_BindingID);
    }
    std::recursive_mutex _mutex;
    EventListeners _eventListeners;
    RegisteredTypes _registeredTypes;
    Events _eventQueue;
    Callbacks _callbacks;
    TypeToBindings _typeToBinding;
    BindingIDs _freeBindingIDs;
    EventBindingID::type _currentBindingID = 0;
    EventTypeID::type _currentTypeID       = 0;
};

auto& GetEventManager()
{
    static EventManager s_EventManager;
    return s_EventManager;
}
}

void TabGraph::Events::Update() { GetEventManager().Update(); }

TabGraph::EventTypeID TabGraph::Events::RegisterType(const EventTypeID& a_Hint) { return GetEventManager().RegisterType(a_Hint); }
TabGraph::EventBindingID TabGraph::Events::BindCallback(const EventTypeID& a_TypeID, const EventCallback& a_Callback, std::any a_UserData) { return GetEventManager().BindCallback(a_TypeID, a_Callback, a_UserData); }
void TabGraph::Events::UnbindCallback(const EventBindingID& a_BindingID) { return GetEventManager().UnbindCallback(a_BindingID); }
void TabGraph::Events::Push(std::unique_ptr<Event>& a_Event) { return GetEventManager().Push(a_Event); }
std::unique_ptr<TabGraph::Event> TabGraph::Events::Poll() { return GetEventManager().Poll(); }
void TabGraph::Events::Consume() { return GetEventManager().Consume(); }
void TabGraph::Events::SetEventListener(const uint32_t& a_EventType, const EventListenerCallback& a_Listener, std::any a_UserData) { return GetEventManager().SetEventListener(SDL_EventType(a_EventType), a_Listener, a_UserData); }
void TabGraph::Events::RemoveEventListener(const uint32_t& a_EventType) { return GetEventManager().RemoveEventListener(SDL_EventType(a_EventType)); }
void TabGraph::Events::PushNoLock(std::unique_ptr<Event>& a_Event) { return GetEventManager().PushNoLock(a_Event); }
