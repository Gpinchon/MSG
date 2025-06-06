#pragma once

#include <functional>
#include <future>
#include <memory_resource>
#include <queue>
#include <thread>

namespace MSG {
class WorkerThread {
public:
    using Task = std::function<void()>;
    inline ~WorkerThread()
    {
        {
            std::unique_lock lock(_mtx);
            _stop = true;
        }
        _cv.notify_one();
    }
    template <typename T>
    inline auto Enqueue(T task) -> std::future<decltype(task())>
    {
        std::future<decltype(task())> future;
        {
            auto wrapper = new std::packaged_task<decltype(task())()>(std::move(task));
            future       = wrapper->get_future();
            std::unique_lock lock(_mtx);
            _tasks.emplace([wrapper] {
                (*wrapper)();
                delete wrapper;
            });
        }
        _cv.notify_one();
        return future;
    }
    inline void PushSynchronousCommand(const Task& a_Command)
    {
        Enqueue(a_Command).wait();
    }
    inline void PushCommand(const Task& a_Command)
    {
        Enqueue(a_Command);
    }
    inline void Wait()
    {
        PushSynchronousCommand([] { /*Push an empty synchronous command to wait for the thread to be done*/ });
    }
    inline auto GetId() const
    {
        return _thread.get_id();
    }
    inline auto PendingTaskCount()
    {
        std::unique_lock lock(_mtx);
        return _tasks.size();
    }

private:
    std::mutex _mtx;
    std::condition_variable _cv;
    std::pmr::unsynchronized_pool_resource _memoryPool;
    std::queue<Task, std::pmr::deque<Task>> _tasks { &_memoryPool };
    bool _stop { false };
    std::jthread _thread { [this] {
        while (true) {
            Task task;
            {
                std::unique_lock lock(_mtx);
                _cv.wait(lock, [this] {
                    return !_tasks.empty() || _stop;
                });
                if (_stop && _tasks.empty())
                    break;
                task = std::move(_tasks.front());
                _tasks.pop();
            }
            task();
        }
    } };
};
}
