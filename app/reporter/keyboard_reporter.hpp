#pragma once

#include <reporter.hpp>

#if TYPECOUNTER_VERSION == 2026000100ull

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

namespace zuu {

class KeyboardRecord;

class KeyboardReporter
 : public zuu::Reporter<KeyboardReporter, KeyboardRecord> {
public:
	static constexpr auto COMPONENT_ID = 0;
	static constexpr const char* STORE_AT = "records";

	[[nodiscard]] static KeyboardReporter& GetInstance() noexcept;

	~KeyboardReporter();

	[[nodiscard]] zuu::fs::path provide_name() const;
	void handle_event(WPARAM wp, LPARAM lp) noexcept;

private:
#ifdef _WIN32
	HHOOK hook_{NULL};
#endif // _WIN32

	KeyboardReporter();
	void install() noexcept;
	void uninstall() noexcept;

#ifdef _WIN32
	static LRESULT CALLBACK hook(int nc, WPARAM wp, LPARAM lp);
#endif // _WIN32
};

} // namespace zuu
#endif // TYPECOUNTER_VERSION == 2026000100ull