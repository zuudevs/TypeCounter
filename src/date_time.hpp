#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

#ifdef _WIN32
    #define NOMINMAX
    #include <Windows.h>
#endif

namespace zuu {

class DateTime {
private:
    using Clock = std::chrono::system_clock;
    using TimePoint = Clock::time_point;

    TimePoint timePoint_{};

    [[nodiscard]]
    static std::tm localTime(const std::time_t time) noexcept {
        std::tm result{};

#ifdef _WIN32
        ::localtime_s(&result, &time);
#else
        ::localtime_r(&time, &result);
#endif

        return result;
    }

public:
	DateTime() noexcept = default;
	DateTime(const DateTime&) noexcept = default;
	DateTime& operator=(const DateTime&) noexcept = default;
	DateTime(DateTime&&) noexcept = default;
	DateTime& operator=(DateTime&&) noexcept = default;
	~DateTime() = default;

	bool operator==(const DateTime& dt) const noexcept {
		return timePoint_ == dt.timePoint_;
	}

    void now() noexcept {
        timePoint_ = Clock::now();
    }

    [[nodiscard]]
    std::string datetime(const char* fmt = "%d-%m-%Y %H:%M:%S") const {
        const auto time = Clock::to_time_t(timePoint_);
        const auto local = localTime(time);

        std::ostringstream oss;
        oss << std::put_time(&local, fmt);

        return oss.str();
    }

    [[nodiscard]]
    std::string date(const char* fmt = "%d-%m-%Y") const {
        const auto time = Clock::to_time_t(timePoint_);
        const auto local = localTime(time);

        std::ostringstream oss;
        oss << std::put_time(&local, fmt);

        return oss.str();
    }

    [[nodiscard]]
    std::string time(const char* fmt = "%H:%M:%S") const {
        const auto time = Clock::to_time_t(timePoint_);
        const auto local = localTime(time);

        std::ostringstream oss;
        oss << std::put_time(&local, fmt);

        return oss.str();
    }

    [[nodiscard]]
    std::string milliseconds() const {
        const auto duration = timePoint_.time_since_epoch();

        const auto ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(duration)
            % 1000;

        std::ostringstream oss;
        oss << std::setfill('0')
            << std::setw(3)
            << ms.count();

        return oss.str();
    }

    [[nodiscard]]
    std::string datetimeMs(
        const char* fmt = "%d-%m-%Y %H:%M:%S"
    ) const {
        const auto time = Clock::to_time_t(timePoint_);
        const auto local = localTime(time);

        const auto duration = timePoint_.time_since_epoch();

        const auto ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(duration)
            % 1000;

        std::ostringstream oss;

        oss << std::put_time(&local, fmt)
            << '.'
            << std::setfill('0')
            << std::setw(3)
            << ms.count();

        return oss.str();
    }

    [[nodiscard]]
    bool isSameDay(const DateTime& other) const noexcept {
        const auto a = localTime(Clock::to_time_t(timePoint_));
        const auto b = localTime(Clock::to_time_t(other.timePoint_));

        return a.tm_year == b.tm_year &&
               a.tm_mon  == b.tm_mon  &&
               a.tm_mday == b.tm_mday;
    }
};

} // namespace zuu