#pragma once

#include <condition_variable>
#include <mutex>

namespace Msg {
class OGLFence {
public:
    OGLFence(const bool& a_DefaultState = false);
    /**
     * @brief sets the fence's status to signaled
     * and notify all waiting threads
     *
     */
    void Signal();
    /**
     * @brief blocks until the fence gets signaled
     *
     */
    void Wait();
    /**
     * @brief resets the fence to the unsignaled status.
     * Has no effect if fence in already unsignaled
     *
     */
    void Reset();
    /**
     * @brief Get the fence's status
     *
     * @return true when the fence is signaled
     * @return false when the fence is unsignaled
     */
    bool GetStatus();

private:
    bool _status;
    std::mutex _mutex;
    std::condition_variable _cv;
};
}
