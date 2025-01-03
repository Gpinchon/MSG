#pragma once

#include <Events/Event.hpp>

#include <functional>
#include <memory>

namespace TabGraph {
TABGRAPH_STRONG_TYPEDEF(EventBindingID, unsigned);
using EventCallback = std::function<void(const Event&, const EventBindingID&, void*)>;
}

namespace TabGraph::Events {
EventTypeID RegisterType(const EventTypeID& a_Hint);
EventBindingID BindCallback(const EventTypeID& a_TypeID, const EventCallback& a_Callback, void* a_UserData = nullptr);
void UnbindCallback(const EventBindingID& a_BindingID);
void Push(std::unique_ptr<Event>& a_Event);
std::unique_ptr<Event> Poll();
}