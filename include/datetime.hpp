#pragma once

#include <version.hpp>

#if TYPECOUNTER_VERSION >= 2026000100ull

#include <chrono>
#include <ctime>
#include <string>

namespace zuu {

class Datetime {
public:
	Datetime() noexcept;

	// === Factory API
	[[nodiscard]] static Datetime Now() noexcept;
	[[nodiscard]] static Datetime FromEpoch(uint64_t ms) noexcept;

	// === Public API
	[[nodiscard]] std::string get_datetime_str() const noexcept;
	[[nodiscard]] std::string get_date_str() const noexcept;
	[[nodiscard]] std::string get_timestamp() const noexcept;
	[[nodiscard]] uint64_t get_epoch() const noexcept;
	void now() noexcept;

private:
	using ClockT = std::chrono::system_clock;
	using TimePoint = ClockT::time_point;

	TimePoint time_point_;

	// === Private API
	[[nodiscard]] std::tm get_local_tm() const noexcept;
};

} // namespace zuu

#endif // TYPECOUNTER_VERSION >= 2026000100ull