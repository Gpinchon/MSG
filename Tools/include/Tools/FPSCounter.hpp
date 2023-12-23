#pragma once

#include <chrono>
#include <iostream>

class FPSCounter {
public:
    FPSCounter(const size_t& a_SampleCount = 1000)
        : alpha(1.0 / double(a_SampleCount))
    {
    }
    void StartFrame()
    {
        startTime = std::chrono::high_resolution_clock::now();
    }
    void EndFrame()
    {
        const auto now          = std::chrono::high_resolution_clock::now();
        const auto newFrameTime = std::chrono::duration<double, std::milli>(now - startTime).count();
        meanFrameTime           = (alpha * newFrameTime) + (1 - alpha) * meanFrameTime;
        fps                     = 1000.0 / meanFrameTime;
        if (fps > 1)
            alpha = 1.0 / fps;
    }
    void Print() const
    {
        std::cout << "\rFPS : " << fps << std::flush;
    }
    std::chrono::high_resolution_clock::time_point startTime;
    double alpha { 1 / 60.0 };
    double meanFrameTime { 0 };
    double fps { 0 };
};
