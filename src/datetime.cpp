#include <datetime.hpp>

#if TYPECOUNTER_VERSION >= 2026000100ull

#include <cstdint>

namespace zuu {

// === Public API

Datetime::Datetime() noexcept = default;

Datetime Datetime::Now() noexcept {
	Datetime dt;
	dt.now();
	return dt;
}

Datetime Datetime::FromEpoch(uint64_t ms) noexcept {
	Datetime dt;
	auto ms_ = std::chrono::milliseconds(ms);
	dt.time_point_ = TimePoint(ms_);
	return dt;
}

std::string Datetime::get_datetime_str() const noexcept {
	auto local = get_local_tm();
	char buffer[24]{};

	std::strftime(
		buffer, 
		sizeof(buffer), 
		"%d-%m-%Y %H:%M:%S", 
		&local
	);

	return std::string(buffer);
}

std::string Datetime::get_date_str() const noexcept {
	auto local = get_local_tm();
	char buffer[12]{};

	std::strftime(
		buffer, 
		sizeof(buffer), 
		"%d-%m-%Y", 
		&local
	);

	return std::string(buffer);
}

std::string Datetime::get_timestamp() const noexcept {
	auto local = get_local_tm();
	char buffer[24]{};

	std::strftime(
		buffer,
		sizeof(buffer),
		"%d-%m-%Y %H:%M:%S.",
		&local
	);

	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_point_.time_since_epoch()).count() % 1000;
	auto val = [=](uint32_t ratio) noexcept -> char {
		return static_cast<char>('0' + (ms / ratio) % 10);
	};

	buffer[20] = val(100);
	buffer[21] = val(10);
	buffer[22] = val(1);

	return std::string(buffer, 23);
}

uint64_t Datetime::get_epoch() const noexcept {
	return time_point_.time_since_epoch().count();
}

void Datetime::now() noexcept {
	time_point_ = ClockT::now();
}

// === Private API

std::tm Datetime::get_local_tm() const noexcept {
    time_t time = ClockT::to_time_t(time_point_);
    std::tm local;
    
#if defined(_WIN32)
    localtime_s(&local, &time); // For Windows
#else
    localtime_r(&time, &local); // For Linux/POSIX
#endif

    return local;
}

#endif // TYPECOUNTER_VERSION >= 2026000100ull

} // namespace zuu