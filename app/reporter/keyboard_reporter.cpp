#include <reporter/keyboard_reporter.hpp>
#include <logger.hpp>

#ifndef NDEBUG
#include <iostream>
#endif // NDEBUG

namespace zuu {

#ifdef _WIN32
#pragma pack(push, 1)
struct KeyboardRecord {
    uint64_t epoch;     // 8 bytes
    uint32_t keycode;   // 4 bytes (VK_CODE)
    uint8_t  action;    // 1 byte (0 = KeyUp, 1 = KeyDown)
};
#pragma pack(pop)
#endif // _WIN32

KeyboardReporter& KeyboardReporter::GetInstance() noexcept {
	static KeyboardReporter instance;
	return instance;
}

KeyboardReporter::~KeyboardReporter() {
	uninstall();
}

zuu::fs::path KeyboardReporter::provide_name() const {
	auto dir = prepare_directory(STORE_AT); 
	std::string filename = zuu::Datetime::Now().get_date_str() + "_keyboard.bin";
	return dir / filename;
}

void KeyboardReporter::handle_event(WPARAM wp, LPARAM lp) noexcept {
#ifdef _WIN32
	const KBDLLHOOKSTRUCT* keyboard_ptr =
	reinterpret_cast<KBDLLHOOKSTRUCT*>(lp);

	const bool is_key_down = (wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN);
	const bool is_key_up   = (wp == WM_KEYUP || wp == WM_SYSKEYUP);

	if (is_key_down) {
#ifndef NDEBUG
		std::clog << zuu::Datetime::Now().get_datetime_str()
				<< " [PRESS] virtual code: " << keyboard_ptr->vkCode << '\n';
#endif // NDEBUG
		push_record(KeyboardRecord{
			zuu::Datetime::Now().get_epoch(),
			keyboard_ptr->vkCode,
			1
		});
	} else if (is_key_up) {
#ifndef NDEBUG
		std::clog << zuu::Datetime::Now().get_datetime_str()
				<< " [RELEASE] virtual code: " << keyboard_ptr->vkCode
				<< '\n';
#endif // NDEBUG
		push_record(KeyboardRecord{
			zuu::Datetime::Now().get_epoch(),
			keyboard_ptr->vkCode,
			0
		});
	}
#endif // _WIN32
}

KeyboardReporter::KeyboardReporter() {
	uint64_t recovered_records{};
	auto& log = zuu::Logger::GetInstance();

	if (!ensure_header(provide_name(), recovered_records)) {
		log.critical("KeyboardReporter: Failed to initialize or recover header!");
	} else {
		log.info("KeyboardReporter: Ready! Record recovered: " + std::to_string(recovered_records));
	}

	install();
}

void KeyboardReporter::install() noexcept {
	hook_ = SetWindowsHookEx(
	WH_KEYBOARD_LL, hook, GetModuleHandle(NULL), 0);

	if (!hook_) {
		zuu::Logger::GetInstance().error("KeyboardReporter: Failed to install Keyboard Hook.");
	}

	zuu::Logger::GetInstance().info("KeyboardReporter: Hook installed successfully.");
}

void KeyboardReporter::uninstall() noexcept {
	if (hook_) {
		UnhookWindowsHookEx(hook_);
		hook_ = NULL;
		zuu::Logger::GetInstance().info("KeyboardReporter: Hook uninstalled.");
	}
}

#ifdef _WIN32
LRESULT CALLBACK KeyboardReporter::hook(int nc, WPARAM wp, LPARAM lp) {
	if (nc == HC_ACTION) {
		KeyboardReporter::GetInstance().handle_event(wp, lp);
	}

	return CallNextHookEx(NULL, nc, wp, lp);
}
#endif // _WIN32

} // namespace zuu