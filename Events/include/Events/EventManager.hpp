#pragma once

#include <Events/Event.hpp>

#include <any>
#include <cstdint>
#include <functional>
#include <memory>

namespace TabGraph {
TABGRAPH_STRONG_TYPEDEF(EventBindingID, uint64_t);
using EventCallback = std::function<void(const Event&, const EventBindingID&, std::any)>;
}

namespace TabGraph::Events {
/// @brief Mandatory before calling Consume or Poll, could be used to deliver events to event listeners by the backend
void Update();

/// @brief Registers a new event type
/// @param a_Hint the implementation will try using this ID, but it might use anything else
/// @return a unique Event ID
EventTypeID RegisterType(const EventTypeID& a_Hint = EventTypeNone);

/// @brief Binds a callback to the specified event type
/// @param a_TypeID the type of the event to bind the callback to
/// @param a_Callback the callback
/// @param a_UserData a user data that will be delivered to the callback
/// @return a unique binding id, used to unbind the callback
EventBindingID BindCallback(const EventTypeID& a_TypeID, const EventCallback& a_Callback, std::any a_UserData = {});

/// @brief Unbinds a callback
/// @param a_BindingID a unique binding id returned by BindCallback
void UnbindCallback(const EventBindingID& a_BindingID);

/// @brief Push a new event that will be consumed on next Consume call
/// @param a_Event the new event to push to the queue
void Push(std::unique_ptr<Event>& a_Event);

/// @brief Returns the first event of the queue or nullptr if the queue is empty
std::unique_ptr<Event> Poll();

/// @brief Consumes every events inside queue, calling the corresponding callbacks
void Consume();
}