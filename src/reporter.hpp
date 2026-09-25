#pragma once

#include "date_time.hpp"
#include "logger.hpp"
#include <filesystem>
#include <fstream>
#include <functional>

#ifndef NS_ABRV_GUARD
#define NS_ABRV_GUARD
#endif // NS_ABRV_GUARD

#ifdef NS_ABRV_GUARD

namespace fs = std::filesystem;

#endif // NS_ABRV_GUARD

namespace zuu {

class Reporter {
public:
	explicit Reporter(Logger& logger)
	 : logger_(logger) {}
	virtual ~Reporter() = default;
	virtual inline void initialize() = 0;

private:
	DateTime openedAt_;

protected:
	Logger& logger_;
	std::fstream handle_;

	[[nodiscard]] inline bool isOpen() const noexcept {
		return handle_.is_open();
	}

	[[nodiscard]] inline std::string filenameExt(const char* ext = "json") const noexcept {
		DateTime dt;
		dt.now();

		return dt.date() + "." + ext;
	}

	[[nodiscard]] inline fs::path prepareDirectory(const char* path) const {
		fs::path absPath = fs::current_path() / path;

		if (!fs::exists(absPath)) {
			fs::create_directories(absPath);
		}

		return absPath;
	}

	bool tryOpen(const char* fullpath, std::ios_base::openmode mode = std::ios::in | std::ios::out | std::ios::binary) {
		auto dt = DateTime::Now();

		handle_.open(fullpath, mode);

		if (!handle_.is_open()) {
			dt.now();
			logger_.write(dt.datetime(), "Info", "Creating new file: " + std::string(fullpath));
            
            std::ofstream creator(fullpath, std::ios::binary);
            creator.close();
            handle_.open(fullpath, mode);
        }

        if (!handle_.is_open()) {
            dt.now();
            logger_.write(dt.datetime(), "Error", "Failed to open " + std::string(fullpath));
            return false;
        }

		openedAt_ = dt;
		return true;
	}

	void close(std::function<void()> callback = [](){}) noexcept {
        if (!handle_.is_open()) {
            return;
        }

		callback();
        handle_.close();

		DateTime dt;

        dt.now();
		std::string msg = dt.date() + ".json closed successfully";
        logger_.write(dt.datetime(), "Info", msg);
    }
	
	[[nodiscard]] inline bool hasContent(const char* targetFile) const noexcept {
        std::ifstream test(targetFile);
        return test.is_open() && test.peek() != std::ifstream::traits_type::eof();
    }

	[[nodiscard]] inline bool isSameDay(const DateTime& dt) const noexcept {
		return openedAt_.isSameDay(dt);
	}

	inline void updateDay(const DateTime& dt) noexcept {
		openedAt_ = dt;
	}
};

} // namespace zuu

#ifdef NS_ABRV_GUARD
#undef NS_ABRV_GUARD
#endif // NS_ABRV_GUARD