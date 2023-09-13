#ifndef StopWatch_HPP
#define StopWatch_HPP
#include <chrono>
#include <iostream>

class StopWatch {
   
public:
    std::chrono::time_point<std::chrono::system_clock> startTime;
    std::chrono::time_point<std::chrono::system_clock> endTime;
    StopWatch();

    void StartStopWatch();
    void StopStopWatch();
    void Reset();

    double GetTime() const;

    StopWatch(const StopWatch&);
    StopWatch& operator=(const StopWatch&);
};

#endif 
