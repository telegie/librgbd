/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#include "time.hpp"

namespace rgbd
{
TimeDuration::TimeDuration(const std::chrono::duration<int64_t, std::micro> duration) noexcept
    : duration_{duration}
{
}

float TimeDuration::seconds() const noexcept
{
    return duration_.count() * 0.000001f;
}

float TimeDuration::milliseconds() const noexcept
{
    return duration_.count() * 0.001f;
}

float TimeDuration::microseconds() const noexcept
{
    return duration_.count() * 1.0f;
}

int64_t TimeDuration::microsecond_count() const noexcept
{
    return duration_.count();
}

TimePoint::TimePoint(
    const std::chrono::time_point<std::chrono::steady_clock, std::chrono::microseconds>
        chrono_time_point) noexcept
    : chrono_time_point_{chrono_time_point}
{
}

TimePoint::TimePoint() noexcept
    : chrono_time_point_{}
{
}

TimePoint TimePoint::now() noexcept
{
    return std::chrono::time_point_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now());
}

TimeDuration TimePoint::elapsed_time() const noexcept
{
    return TimeDuration{TimePoint::now() - chrono_time_point_};
}

TimeDuration TimePoint::operator-(const TimePoint& other) const noexcept
{
    return TimeDuration{chrono_time_point_ - other.chrono_time_point_};
}

IntervalTimer::IntervalTimer(float interval_seconds)
    : interval_seconds_{interval_seconds}
    , time_point_{TimePoint::now()}
{
}
bool IntervalTimer::check()
{
    if (time_point_.elapsed_time().seconds() < interval_seconds_)
        return false;

    time_point_ = TimePoint::now();
    return true;
}
} // namespace kr
