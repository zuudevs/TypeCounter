#pragma once

#include "config.hpp"
#include "date_time.hpp"
#include "logger.hpp"
#include "reporter.hpp"
#include <algorithm>
#include <fstream>
#include <ios>
#include <string>
#include <filesystem>

namespace zuu {

namespace detail_ {

enum class Status : unsigned char { 
    Error    = 0,
    Idle     = 1 << 0, 
    Running  = 1 << 1, 
    Continue = 1 << 2
};

inline constexpr Status operator|(Status a, Status b) noexcept {
    return static_cast<Status>(static_cast<unsigned char>(a) | static_cast<unsigned char>(b));
}

inline constexpr Status operator&(Status a, Status b) noexcept {
    return static_cast<Status>(static_cast<unsigned char>(a) & static_cast<unsigned char>(b));
}

inline constexpr Status operator~(Status a) noexcept {
    return static_cast<Status>(~static_cast<unsigned char>(a));
}

inline Status& operator|=(Status& a, Status b) noexcept { return a = a | b; }
inline Status& operator&=(Status& a, Status b) noexcept { return a = a & b; }

} // namespace detail_

class KeyInputCollector : public Reporter {
public:
	using Status = detail_::Status;
	static constexpr const char* stored_at = "data/history/keyboard";

    explicit KeyInputCollector(Logger& logger) noexcept
     : Reporter(logger), status_(Status::Idle) {}

    inline void initialize() {
		auto basePath = prepareDirectory(stored_at);
		auto fullpath = basePath / filenameExt();
        bool fileExist = hasContent(fullpath.string().c_str());
		auto dt = DateTime::Now();

        logger_.write(dt.datetime(), "Info", "Initializing user type storage");
        handle_.open(fullpath, std::ios::in | std::ios::out | std::ios::binary);

		status_ = ((tryOpen(fullpath.string().c_str(), std::ios::out | std::ios::out | std::ios::binary) == 0) ? Status::Error : Status::Running);

		if (status_ == Status::Error) {
			return;
		}

		dt.now();
		std::string msg = "Successfully create " + dt.date() + ".json";
		logger_.write(dt.datetime(), "Info", msg);

        if (!fileExist) {
            handle_ << "[\n";
        } else {
            prepareAppendOnExistingRecord();
        }
    }

    void pushRecord(unsigned vKey, bool isKeyUp) noexcept {
		DateTime dt;

        if (!isOpen()) {
			dt.now();
			logger_.write(dt.datetime(), "Warning", "Attempted to push record but file is closed");
			return;
		}

        if (hasStatus(Status::Continue)) {
            handle_ << ",\n";
        }
        
        handle_ << "  {\n"
                << "    \"method\": \"" << (isKeyUp ? "Key Up" : "Key Down") << "\",\n"
                << "    \"code\": " << vKey << ",\n"
                << "    \"timestamp\": \"" << dt.datetime("%d-%m-%Y %H:%M:%S.s")  << "\"\n"
                << "  }";
        
        handle_ << std::flush;

		if (handle_.fail()) {
			dt.now();
            logger_.write(dt.datetime(), "Error", "Failed to write record to disk (disk full?)");
			status_ = Status::Error;
        } else {
			addStatus(Status::Continue);
		}
    }

    void close() noexcept {
		Reporter::close([&]() {
			handle_ << "\n]\n" << std::flush;
		});

		status_ = Status::Idle;
    }

    void rolloverIfNewDay() {
        DateTime dt;
        dt.now();

        if (!isSameDay(dt)) {
			logger_.write(dt.datetime(), "Info", "Day changed, triggering storage rollover");
			
            close();
            initialize();
        }
    }

	[[nodiscard]] inline bool hasStatus(Status flag) const noexcept {
        if (flag == Status::Error) {
            return status_ == Status::Error;
        }
        return (status_ & flag) != Status::Error; 
    }

private:
	Status status_;

    void prepareAppendOnExistingRecord() {
        handle_.seekg(0, std::ios::end);
        const std::streampos length = handle_.tellg();

        if (length > 0) {
            char c;
            bool foundBracket = false;
            std::streampos bracketPos{};

            for (std::streampos i = 1; i <= length; i += 1) {
                handle_.seekg(-i, std::ios::end);
                c = static_cast<char>(handle_.peek());
                if (c == ']') {
                    foundBracket = true;
                    bracketPos = handle_.tellg();
                    break;
                }
            }

            handle_.clear();

            if (foundBracket) {
                handle_.seekp(bracketPos);
                addStatus(Status::Continue);
            } else {
                handle_.seekp(0, std::ios::end);

				DateTime dt;

                dt.now();
                logger_.write(dt.datetime(), "Warning", "Malformed JSON detected (missing ']'), resetting append point");
            }
        }
    }

	inline void addStatus(Status flag) noexcept { 
        status_ |= flag; 
    }
    
    inline void removeStatus(Status flag) noexcept { 
        status_ &= ~flag; 
    }
};

} // namespace zuu