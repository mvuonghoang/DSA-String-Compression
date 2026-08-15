#ifndef TIMER_H
#define TIMER_H

#include <chrono>

/**
 * @brief High-precision timer using std::chrono for measuring algorithm execution time in milliseconds.
 */
class Timer {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    bool isRunning;

public:
    Timer() : isRunning(false) {}

    void start() {
        startTime = std::chrono::high_resolution_clock::now();
        isRunning = true;
    }

    void stop() {
        endTime = std::chrono::high_resolution_clock::now();
        isRunning = false;
    }

    /**
     * @brief Returns elapsed time in milliseconds (ms) with high floating-point precision.
     */
    double elapsedMilliseconds() const {
        auto end = isRunning ? std::chrono::high_resolution_clock::now() : endTime;
        std::chrono::duration<double, std::milli> duration = end - startTime;
        return duration.count();
    }
};

#endif // TIMER_H
