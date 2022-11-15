#pragma once

#include <functional>
#include <condition_variable>
#include <queue>
#include <future>

namespace TabGraph::Tools
{
class WorkerThread
{
public:
    using Task = std::function<void()>;
    inline WorkerThread() {
        _thread = std::thread([this] {
            while (true) {
                Task task;
                {
                    std::unique_lock<std::mutex> lock(_mtx);
                    _cv.wait(lock, [this] {
                        return !_tasks.empty() || _stop;
                        });
                    if (_stop && _tasks.empty()) break;
                    task = std::move(_tasks.front());
                    _tasks.pop();
                }
                task();
            }
        });
    }
    inline ~WorkerThread() {
        {
            std::unique_lock<std::mutex> lock(_mtx);
            _stop = true;
        }
        _cv.notify_one();
        _thread.join();
    }
    template<typename T>
    inline auto Enqueue(T task) -> std::future<decltype(task())> {
        std::future<decltype(task())> future;
        {
            auto wrapper = new std::packaged_task<decltype(task()) ()>(std::move(task));
            future = wrapper->get_future();
            std::unique_lock<std::mutex> lock(_mtx);
            _tasks.emplace([wrapper] {
                (*wrapper)();
                delete wrapper;
            });
        }
        _cv.notify_one();
        return future;
    }
    inline void PushCommand(const Task& a_Command, const bool a_Synchronous)
    {
        if (a_Synchronous) Enqueue(a_Command).get();
        else Enqueue(a_Command);
    }
    //Pushes an empty synchronous command to wait for the thread to be done
    inline void Wait()
    {
        PushCommand([]{}, true);
    }
private:
    std::mutex              _mtx;
    std::condition_variable _cv;
    std::queue<Task>        _tasks;

    std::thread _thread;
    bool _stop{ false };
};
}