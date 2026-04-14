#pragma once

#include <functional>
#include <future>
#include <iostream>
#include <memory_resource>
#include <queue>
#include <thread>

namespace Msg {
class WorkerThread {
public:
    using Task = std::move_only_function<void()>;
    inline ~WorkerThread()
    {
        {
            std::lock_guard lock(_mtx);
            _stop = true;
        }
        _cv.notify_one();
    }
    template <typename Func>
    inline auto Enqueue(Func&& task) -> std::future<std::invoke_result_t<Func>>
    {
        using Result = std::invoke_result_t<Func>;
        auto wrapper = std::make_unique<std::packaged_task<Result()>>(std::forward<Func>(task));
        auto future  = wrapper->get_future();
        {
            std::lock_guard lock(_mtx);
            _tasks.emplace([wrapper = std::move(wrapper)] {
                (*wrapper)();
            });
        }
        _cv.notify_one();
        return future;
    }
    template <typename Func>
    inline void PushSynchronousCommand(Func&& a_Command)
    {
        Enqueue(std::forward<Func>(a_Command)).wait();
    }
    template <typename Func>
    inline void PushCommand(Func&& a_Command)
    {
        Enqueue(std::forward<Func>(a_Command));
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
        std::lock_guard lock(_mtx);
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
            if (!task)
                continue;
            try {
                task();
            } catch (const std::exception& e) {
                std::cerr << "WorkerThread task threw: " << e.what() << '\n';
            }
        }
    } };
};
}
