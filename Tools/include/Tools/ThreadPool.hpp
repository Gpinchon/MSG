#pragma once

#include <Tools/WorkerThread.hpp>

#include <vector>

namespace TabGraph::Tools {
class ThreadPool {
public:
    using Task = std::function<void()>;
    ThreadPool(const size_t& a_ThreadCount = std::thread::hardware_concurrency())
        : _workerThreads(a_ThreadCount)
    {
    }
    template <typename T>
    inline auto Enqueue(T task) -> std::future<decltype(task())>
    {
        while (_workerThreads[_CurrentThread()].GetId() == std::this_thread::get_id())
            _NextCurrentThread();
        auto future = _workerThreads[_CurrentThread()].Enqueue(task);
        _NextCurrentThread();
        return future;
    }
    inline void PushCommand(const Task& a_Command, const bool a_Synchronous)
    {
        if (a_Synchronous)
            a_Command();
        else
            Enqueue(a_Command);
    }

private:
    inline void _NextCurrentThread()
    {
        _currentThread = ++_currentThread % _workerThreads.size();
    }
    inline size_t _CurrentThread() const
    {
        return _currentThread;
    }
    size_t _currentThread { 0 };
    std::vector<Tools::WorkerThread> _workerThreads {};
};
}
