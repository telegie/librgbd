/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#pragma once

#include <chrono>

namespace rgbd
{
class TimeDuration
{
public:
    TimeDuration(std::chrono::duration<int64_t, std::micro> duration) noexcept;
    float seconds() const noexcept;
    float milliseconds() const noexcept;
    float microseconds() const noexcept;
    int64_t microsecond_count() const noexcept;

private:
    const std::chrono::duration<int64_t, std::micro> duration_;
};

class TimePoint
{
public:
    TimePoint(std::chrono::time_point<std::chrono::steady_clock, std::chrono::microseconds>
                  chrono_time_point) noexcept;
    TimePoint() noexcept;
    TimePoint(const TimePoint& other) = default;
    TimePoint(TimePoint&& other) = default;
    TimePoint& operator=(const TimePoint& other) = default;
    TimePoint& operator=(TimePoint&& other) = default;
    static TimePoint now() noexcept;
    TimeDuration elapsed_time() const noexcept;
    TimeDuration operator-(const TimePoint& other) const noexcept;

private:
    std::chrono::time_point<std::chrono::steady_clock, std::chrono::microseconds>
        chrono_time_point_;
};

class IntervalTimer
{
public:
    IntervalTimer(float interval_seconds);
    bool check();

private:
    float interval_seconds_;
    TimePoint time_point_;
};
} // namespace kr
