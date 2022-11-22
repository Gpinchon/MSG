#pragma once

#include <chrono>
#include <iostream>

struct ScoppedTimer {
    using period = std::milli;
    ScoppedTimer(const std::string& a_Name, std::ostream& a_Ostream = std::cout)
        : name(a_Name)
        , ostream(a_Ostream)
    {}
    ~ScoppedTimer() {
        Print();
    }
    void Print() const {
        ostream << name << " took " << Elapsed().count() << " ms\n";
    }
    std::chrono::duration<double, period> Elapsed() const {
        return (std::chrono::steady_clock::now() - start);
    }
    const std::string name;
    const std::chrono::steady_clock::time_point start{ std::chrono::steady_clock::now() };
    std::ostream& ostream;
};