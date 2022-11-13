/*
* @Author: gpinchon
* @Date:   2021-05-31 14:16:06
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:12:47
*/

#include <Config.hpp>
#include <Events/DispatchQueue.hpp>
#include <Events/Manager.hpp>

namespace TabGraph::Events {
DispatchQueue& DispatchQueue::ApplicationDispatchQueue()
{
    static DispatchQueue s_dispatchQueue(Config::Global().Get("DispatchQueueThreadsNbr", std::thread::hardware_concurrency()));
    return s_dispatchQueue;
}

void DispatchQueue::_DispatchThreadHandler(void)
{
    std::unique_lock<std::mutex> lock(_lock);
    while (_running) {
        _cv.wait(lock, [this] {
            return _queue.size() || !_running;
        });
        if (_queue.empty())
            continue;
        auto task { std::move(_queue.front()) };
        auto fun { task.first };
        auto taskID { task.second };
        _queue.pop();
        lock.unlock();
        fun();
        Event event;
        event.type = Event::Type::TaskComplete;
        event.data = Event::TaskComplete { _ID, taskID };
        Manager::PushEvent(event);
        lock.lock();
    }
}
}