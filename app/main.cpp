#include <logger.hpp>
#include <reporter/keyboard_reporter.hpp>

int main() {
	auto& log = zuu::Logger::GetInstance();
    auto& reporter = zuu::KeyboardReporter::GetInstance();

#ifdef _WIN32
	MSG msg{};
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
#endif // _WIN32

	return 0;
}