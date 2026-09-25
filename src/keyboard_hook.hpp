#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <functional>

#include "key_input_collector.hpp"

namespace zuu {

class KeyboardHook {
public:
	KeyboardHook(KeyInputCollector& collector, std::function<void()> exitRequest)
	 : collector_(collector), exitRequested_(exitRequest) {}

	~KeyboardHook() {
        uninstall();
        if (activeInstance_ == this) activeInstance_ = nullptr;
    }

	[[nodiscard]] bool install() {
        hook_ = SetWindowsHookEx(
			WH_KEYBOARD_LL, 
			lowLevelProc, 
			GetModuleHandle(NULL), 
			0
		);
        return hook_ != NULL;
    }

	void uninstall() {
        if (hook_) {
            UnhookWindowsHookEx(hook_);
            hook_ = NULL;
        }
    }

private:
	KeyInputCollector& collector_;
	std::function<void()> exitRequested_;
    HHOOK hook_{NULL};
    bool ctrlPressed_{false};

	static inline KeyboardHook* activeInstance_ = nullptr;

	static LRESULT CALLBACK lowLevelProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode == HC_ACTION && activeInstance_) {
            activeInstance_->handleEvent(wParam, lParam);
        }
        return CallNextHookEx(activeInstance_ ? activeInstance_->hook_ : NULL, nCode, wParam, lParam);
    }
 
    void handleEvent(WPARAM wParam, LPARAM lParam) {
        const KBDLLHOOKSTRUCT* pKeyboard = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
 
        updateCtrlState(pKeyboard->vkCode, wParam);
 
        const bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        const bool isKeyUp   = (wParam == WM_KEYUP   || wParam == WM_SYSKEYUP);
 
        if (isKeyDown) {
            collector_.pushRecord(pKeyboard->vkCode, /*isKeyUp=*/false);
 
            constexpr unsigned kVkC = 0x43; // 'C'
            if (ctrlPressed_ && pKeyboard->vkCode == kVkC) {
                if (exitRequested_) exitRequested_();
            }
        } else if (isKeyUp) {
            collector_.pushRecord(pKeyboard->vkCode, /*isKeyUp=*/true);
        }
    }
 
    void updateCtrlState(DWORD vkCode, WPARAM wParam) noexcept {
        if (vkCode != VK_CONTROL && vkCode != VK_LCONTROL && vkCode != VK_RCONTROL) return;
 
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            ctrlPressed_ = true;
        } else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
            ctrlPressed_ = false;
        }
    }
};

} // namespace zuu