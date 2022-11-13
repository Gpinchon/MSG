/*
* @Author: gpinchon
* @Date:   2021-05-30 22:32:44
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-30 22:33:52
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <functional>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Events {
class DispatchQueue {
public:
    struct TaskIdentifier {
        uint16_t dispatchQueueID{ 0 };
        uint32_t taskID{ 0 };
    };
    DispatchQueue(size_t threadsCount = 1)
        : _threads(threadsCount)
    {
        static uint16_t s_ID{0};
        _ID = ++s_ID;
        for (auto& thread : _threads)
            thread = std::thread(&DispatchQueue::_DispatchThreadHandler, this);
    }
    ~DispatchQueue()
    {
        _running = false;
        _cv.notify_all();
        for (auto& thread : _threads)
            if (thread.joinable())
                thread.join();
    }
    TaskIdentifier Dispatch(std::function<void(void)> fun)
    {
        std::unique_lock<std::mutex> lock(_lock);
        _queue.push({fun, ++_taskID});
        lock.unlock();
        _cv.notify_one();
        return { _ID, _taskID };
    }
    /**
     * @brief use this to dispatch an asynchronous task.
     * Threads Number is defined through Config::Global().Set("DispatchQueueThreadsNbr", <value> (default : 16) ).
     * @return the application's DispatchQueue
    */
    static DispatchQueue& ApplicationDispatchQueue();

private:
    void _DispatchThreadHandler(void);
    uint16_t _ID{ 0 };
    uint32_t _taskID { 0 };
    bool _running { true };
    std::vector<std::thread> _threads;
    std::mutex _lock;
    std::condition_variable _cv;
    std::queue<std::pair<std::function<void(void)>, uint32_t>> _queue;
};
}
