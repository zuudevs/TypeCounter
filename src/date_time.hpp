#pragma once

#include <ctime>
#include <chrono>
#include <iomanip>
#include <string>
#include <sstream>

namespace zuu {

class DateTime {
private:
	using TimePt = std::chrono::steady_clock::time_point;

	time_t time_{};
	unsigned ms_{};

public:
	inline void now() noexcept {
		auto now = std::chrono::system_clock::now();
		time_ = std::chrono::system_clock::to_time_t(now);

		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
		ms_ = ms % 1000;
	}

    [[nodiscard]] inline std::string datetime(const char* fmt = "%d-%m-%Y %H:%M:%S") const noexcept {
        auto localTime = *std::localtime(&time_);

		std::string format_str(fmt);
        bool inject_ms = false;

        auto pos = format_str.find(".%s");
        if (pos != std::string::npos) {
            inject_ms = true;
            format_str.erase(pos, 3); 
        }

		std::ostringstream oss;
        oss << std::put_time(&localTime, fmt);

		if (inject_ms) {
            oss << "." << std::setfill('0') << std::setw(3) << ms_;
        }
        
        return oss.str();
    }

    [[nodiscard]] inline std::string date(const char* fmt = "%d-%m-%Y") const noexcept {
        auto localTime = *std::localtime(&time_);

		std::ostringstream oss;
        oss << std::put_time(&localTime, fmt);
        
        return oss.str();
    }

    [[nodiscard]] inline std::string time(const char* fmt = "%H:%M:%S") const {
        auto localTime = *std::localtime(&time_);
        
        std::string format_str(fmt);
        bool inject_ms = false;

        auto pos = format_str.find(".%s");
        if (pos != std::string::npos) {
            inject_ms = true;
            format_str.erase(pos, 3); 
        }

        std::ostringstream oss;
        oss << std::put_time(&localTime, format_str.c_str());
        
        if (inject_ms) {
            oss << "." << std::setfill('0') << std::setw(3) << ms_;
        }
        
        return oss.str();
    }

	[[nodiscard]] inline bool isSameDay(const DateTime& dt) const noexcept {
		auto a = *std::localtime(&time_);
		auto b = *std::localtime(&dt.time_);

		return a.tm_mday == b.tm_mday && a.tm_mon == b.tm_mon && a.tm_year == b.tm_year;
	}
};

} // namespace zuu