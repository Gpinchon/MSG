#include <MSG/OGLFence.hpp>

Msg::OGLFence::OGLFence(const bool& a_DefaultState)
    : _status(a_DefaultState)
{
}

void Msg::OGLFence::Signal()
{
    // enclose mutex lock so it's unlocked when notifying cv
    {
        auto lock = std::unique_lock(_mutex);
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

void Msg::OGLFence::Reset()
{
    auto lock = std::unique_lock(_mutex);
    _status   = false;
}

bool Msg::OGLFence::GetStatus()
{
    auto lock = std::unique_lock(_mutex);
    return _status;
}