#ifndef StopWatch_CPP
#define StopWatch_CPP

#include <chrono>
#include "StopWatch.hpp"
std::chrono::duration<double> elapsed_seconds_prev{};
std::chrono::duration<double> elapsed_seconds{};

    StopWatch::StopWatch() : startTime(std::chrono::system_clock::now()), endTime(std::chrono::system_clock::now()){}

    void StopWatch::StartStopWatch() {
        startTime = std::chrono::system_clock::now();
    }
    void StopWatch::StopStopWatch() {
        endTime = std::chrono::system_clock::now();
        elapsed_seconds_prev += endTime - startTime;
    }
    void StopWatch::Reset() {
        startTime = endTime = std::chrono::system_clock::now();
        elapsed_seconds_prev = endTime - startTime;
    }

    double StopWatch::GetTime() const {
        return elapsed_seconds_prev.count();
    }

    StopWatch::StopWatch(const StopWatch& sw) : startTime(sw.startTime), endTime(sw.endTime) {}
    StopWatch& StopWatch::operator=(const StopWatch& sw) {
        if (this == &sw) {
            return *this;
        }
        else {
            this->startTime = sw.startTime;
            this->endTime = sw.endTime;
            return *this;
        }
        
    }

#endif