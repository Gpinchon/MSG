#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <queue>

namespace TabGraph::Tools {
/**
 * @brief a worker thread that only executes commands when manually instructed to
*/
class ManualWorkerThread { 
public:
    using Task = std::function<void()>;
    inline ManualWorkerThread()
    {
        _thread = std::thread([this] {
            while (true) {
                Task task;
                {
                    std::unique_lock<std::mutex> lock(_mtx);
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
        });
    }
    inline ~ManualWorkerThread()
    {
        {
            std::unique_lock<std::mutex> lock(_mtx);
            _stop = true;
        }
        _cv.notify_one();
        _thread.join();
    }
    template <typename T>
    inline auto Enqueue(T task) -> std::future<decltype(task())>
    {
        std::future<decltype(task())> future;
        {
            auto wrapper = new std::packaged_task<decltype(task())()>(std::move(task));
            future       = wrapper->get_future();
            std::unique_lock<std::mutex> lock(_mtx);
            _tasks.emplace([wrapper] {
                (*wrapper)();
                delete wrapper;
            });
        }
        return future;
    }
    inline void PushCommand(const Task& a_Command)
    {
        Enqueue(a_Command);
    }
    // Pushes an empty synchronous command to wait for the thread to be done
    inline void Wait()
    {
        Enqueue([] {}).get();
    }
    inline void Execute(const bool& a_Synchronous = false)
    {
        auto future = Enqueue([] {});
        _cv.notify_one();
        if (a_Synchronous) future.get();
    }
    inline auto GetId()
    {
        return _thread.get_id();
    }

private:
    std::mutex _mtx;
    std::condition_variable _cv;
    std::queue<Task> _tasks;

    std::thread _thread;
    bool _stop { false };
};

class WorkerThread {
public:
    using Task = std::function<void()>;
    inline WorkerThread()
    {
        _thread = std::thread([this] {
            while (true) {
                Task task;
                {
                    std::unique_lock<std::mutex> lock(_mtx);
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
        });
    }
    inline ~WorkerThread()
    {
        {
            std::unique_lock<std::mutex> lock(_mtx);
            _stop = true;
        }
        _cv.notify_one();
        _thread.join();
    }
    template <typename T>
    inline auto Enqueue(T task) -> std::future<decltype(task())>
    {
        std::future<decltype(task())> future;
        {
            auto wrapper = new std::packaged_task<decltype(task())()>(std::move(task));
            future       = wrapper->get_future();
            std::unique_lock<std::mutex> lock(_mtx);
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
        Enqueue(a_Command).get();
    }
    inline void PushCommand(const Task& a_Command)
    {
        Enqueue(a_Command);
    }
    // Pushes an empty synchronous command to wait for the thread to be done
    inline void Wait()
    {
        PushSynchronousCommand([] {});
    }
    inline void Execute(const bool& a_Synchronous)
    {
        _cv.notify_one();
    }
    inline auto GetId()
    {
        return _thread.get_id();
    }

private:
    std::mutex _mtx;
    std::condition_variable _cv;
    std::queue<Task> _tasks;

    std::thread _thread;
    bool _stop { false };
};
}
