#include <Events/EventManager.hpp>

#include <memory>
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
using CallbackMap      = std::unordered_map<EventBindingID, CallbackStorage>;
using TypeToBindingMap = std::unordered_map<EventTypeID, std::vector<EventBindingID>>;
using BindingIDQueue   = std::queue<EventBindingID>;
using EventQueue       = std::queue<std::unique_ptr<Event>>;
using RegisteredTypes  = std::unordered_set<EventTypeID>;

class EventManager {
public:
    EventBindingID BindCallback(const EventTypeID& a_TypeID, const EventCallback& a_Callback, std::any a_UserData = {})
    {
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
        _RestoreBindingID(a_BindingID);
        _typeToBinding.erase(_callbacks.at(a_BindingID).eventTypeID);
        _callbacks.erase(a_BindingID);
    }
    EventTypeID RegisterType(const EventTypeID& a_Hint)
    {
        auto newTypeID = a_Hint != EventTypeNone ? a_Hint : EventTypeID(_currentTypeID);
        while (!_registeredTypes.insert(newTypeID).second) // try until we can insert a new Event ID
            newTypeID = EventTypeID(_currentTypeID++);
        return newTypeID;
    }
    void Push(std::unique_ptr<Event>& a_Event)
    {
        _eventQueue.push(std::move(a_Event));
    }
    std::unique_ptr<Event> Poll()
    {
        if (_eventQueue.empty())
            return {};
        auto event = std::move(_eventQueue.back());
        _eventQueue.pop();
        return event;
    }
    void ConsumeEvents()
    {
        for (auto event = Poll(); event != nullptr; event = Poll()) {
            auto bindingIDsItr = _typeToBinding.find(event->typeID);
            if (bindingIDsItr == _typeToBinding.end())
                continue;
            for (auto& bindingID : bindingIDsItr->second) {
                auto& callback = _callbacks.at(bindingID);
                callback.callback(*event, bindingID, callback.userData);
            }
        }
    }

private:
    EventBindingID _GetFreeBindingID()
    {
        EventBindingID newBindingID(_currentBindingID);
        if (!_freeBindingIDs.empty()) {
            newBindingID = _freeBindingIDs.back();
            _freeBindingIDs.pop();
        } else
            _currentBindingID++;
        return newBindingID;
    }
    void _RestoreBindingID(const EventBindingID& a_BindingID)
    {
        _freeBindingIDs.push(a_BindingID);
    }
    RegisteredTypes _registeredTypes;
    EventQueue _eventQueue;
    CallbackMap _callbacks;
    TypeToBindingMap _typeToBinding;
    BindingIDQueue _freeBindingIDs;
    EventBindingID::type _currentBindingID = 0;
    EventTypeID::type _currentTypeID       = 0;
};

auto& GetEventManager()
{
    static EventManager s_EventManager;
    return s_EventManager;
}

EventTypeID RegisterType(const EventTypeID& a_Hint) { return GetEventManager().RegisterType(a_Hint); }
EventBindingID BindCallback(const EventTypeID& a_TypeID, const EventCallback& a_Callback, std::any a_UserData) { return GetEventManager().BindCallback(a_TypeID, a_Callback, a_UserData); }
void UnbindCallback(const EventBindingID& a_BindingID) { return GetEventManager().UnbindCallback(a_BindingID); }
void Push(std::unique_ptr<Event>& a_Event) { return GetEventManager().Push(a_Event); }
std::unique_ptr<Event> Poll() { return GetEventManager().Poll(); }
void ComsumeEvents() { return GetEventManager().ConsumeEvents(); }
}
