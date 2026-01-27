#pragma once
#include <chrono>


namespace land {


class Debouncer {
    using Clock = std::chrono::steady_clock;
    Clock::time_point         mLastTime{};
    std::chrono::milliseconds mInterval;

public:
    explicit Debouncer(int ms) : mInterval(ms) {}

    [[nodiscard]] inline bool ready() {
        auto now = Clock::now();
        if (now - mLastTime >= mInterval) {
            mLastTime = now;
            return true;
        }
        return false;
    }
};


} // namespace land