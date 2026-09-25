#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <utility>

namespace zuu {

class Logger {
public:
	explicit Logger(std::string filename = "system.log") noexcept
	 : filename_(std::move(filename)) {}

	void open() {
		handle_.open(filename_, std::ios::out | std::ios::app);
		if (!handle_.is_open()) {
			std::cerr << "Failed to initialize log file." << std::endl;
		}
	}

	void close() noexcept {
		if (handle_.is_open()) {
			handle_.close();
		}
	}

	[[nodiscard]] inline bool isOpen() const noexcept {
		return handle_.is_open();
	}

	void write(const std::string& datetime, const std::string& tag, const std::string& msg) {
		if (!handle_.is_open()) {
			return;
		}

		handle_ << datetime << " [" << tag << "] " << msg << "\n" << std::flush;
	}

	~Logger() {
		handle_.close();
	}

private:
	std::string filename_;
	std::ofstream handle_;
};

} // namespace zuu