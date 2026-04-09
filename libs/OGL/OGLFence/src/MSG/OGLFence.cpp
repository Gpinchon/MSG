#include <MSG/OGLFence.hpp>

#include <chrono>

Msg::OGLFence::OGLFence(const bool& a_DefaultState)
    : _status(a_DefaultState)
{
}

void Msg::OGLFence::Signal()
{
    // enclose mutex lock so it's unlocked when notifying cv
    {
        auto lock = std::lock_guard(_mutex);
        if (_status) // already signaled
            return;
        _status = true;
    }
    _cv.notify_all();
}

void Msg::OGLFence::Wait()
{
    auto lock = std::unique_lock(_mutex);
    _cv.wait(lock, [&status = _status] { return status; });
}

bool Msg::OGLFence::WaitFor(const uint32_t& a_Nanoseconds)
{
    auto lock = std::unique_lock(_mutex);
    return _cv.wait_for(lock, std::chrono::nanoseconds(a_Nanoseconds), [&status = _status] { return status; });
}

void Msg::OGLFence::Reset()
{
    auto lock = std::lock_guard(_mutex);
    _status   = false;
}

bool Msg::OGLFence::GetStatus()
{
    return WaitFor(1);
}