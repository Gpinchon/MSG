/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:12:49
*/

#include <Events/Event.hpp>
#include <Events/Manager.hpp>

#if MEDIALIBRARY == SDL2
#include <Driver/SDL2/Events/Manager.hpp>
#endif //MEDIALIBRARY == SDL2

namespace TabGraph::Events {
Manager::Manager()
    : _impl(new Manager::Impl)
{
}

Manager& Manager::_Get()
{
    static Manager Manager;
    return Manager;
}

Signal<const Event&>& Manager::On(Event::Type type)
{
    return _Get()._impl->On(type);
}

void Manager::PushEvent(const Event& event)
{
    return _Get()._impl->PushEvent(event);
}

void Manager::PollEvents()
{
    return _Get()._impl->PollEvents();
}
}
