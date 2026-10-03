#pragma once

#include <cstdint>

namespace zuu {

enum class ReporterStatus : uint8_t {
	Idle		= 0,
	Error		= 1 << 0,
	Ready		= 1 << 1,
	Continue	= 1 << 2,
};

inline constexpr ReporterStatus
operator|(ReporterStatus a, ReporterStatus b) noexcept {
    return static_cast<ReporterStatus>(static_cast<unsigned char>(a) |
                                        static_cast<unsigned char>(b));
}
inline constexpr ReporterStatus
operator&(ReporterStatus a, ReporterStatus b) noexcept {
    return static_cast<ReporterStatus>(static_cast<unsigned char>(a) &
                                        static_cast<unsigned char>(b));
}
inline constexpr ReporterStatus
operator~(ReporterStatus a) noexcept {
    return static_cast<ReporterStatus>(~static_cast<unsigned char>(a));
}
inline ReporterStatus&
operator|=(ReporterStatus& a, ReporterStatus b) noexcept {
    return a = a | b;
}
inline ReporterStatus&
operator&=(ReporterStatus& a, ReporterStatus b) noexcept {
    return a = a & b;
}

enum class Component : uint8_t {
	Reporter,
	Logger,
	KeyboardCollector,
	MouseCollector,
};

} // namespace zuu