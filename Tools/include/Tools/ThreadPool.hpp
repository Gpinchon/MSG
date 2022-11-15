#pragma once

#include <Tools/WorkerThread.hpp>

#include <array>

namespace TabGraph::Tools
{
template<size_t ThreadCount>
class ThreadPool {
public:
	using Task = std::function<void()>;
	template<typename T>
	inline auto Enqueue(T task)->std::future<decltype(task())> {
		auto future = _workerThreads[_currentThread].Enqueue(task);
		_currentThread = ++_currentThread % ThreadCount;
		return future;
	}
	inline void PushCommand(const Task& a_Command, const bool a_Synchronous)
	{
		if (a_Synchronous) Enqueue(a_Command).get();
		else Enqueue(a_Command);
	}

private:
	size_t _currentThread{ 0 };
	std::array<Tools::WorkerThread, ThreadCount> _workerThreads{};
};
}