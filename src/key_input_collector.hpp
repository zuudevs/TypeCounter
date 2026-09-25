#pragma once

#include "config.hpp"
#include "date_time.hpp"
#include "logger.hpp"
#include <fstream>
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

class KeyInputCollector {
public:
	using Status = detail_::Status;

    explicit KeyInputCollector(Logger& logger) noexcept
     : logger_(), status_(Status::Idle) {}

    [[nodiscard]] inline bool isOpen() const noexcept {
        return handle_.is_open();
    }

    inline void initialize() {
		DateTime dt;

        dt.now();
		auto collect_dir = std::filesystem::current_path() / records_dir;

		if (!std::filesystem::exists( collect_dir)) {
			std::filesystem::create_directories(collect_dir);
		}

        auto filenameWithExt = dt.date() + ".json";
		auto fullpath = collect_dir / filenameWithExt;
        bool fileExist = hasContent(fullpath.string());

        logger_.write(dt.datetime(), "Info", "Initializing user type storage");
        handle_.open(fullpath, std::ios::in | std::ios::out | std::ios::binary);

        if (!handle_.is_open()) {
            dt.now();
            logger_.write(dt.datetime(), "Info", "Trying to create user type storage");
            
            std::ofstream creator(fullpath, std::ios::binary);
            creator.close();
            handle_.open(fullpath, std::ios::in | std::ios::out | std::ios::binary);
        }

        if (!handle_.is_open()) {
            dt.now();
            logger_.write(dt.datetime(), "Error", "Can't open store file");
			status_ = Status::Error;
            return;
        }

		status_ = Status::Running;
		openedAt_.now();

        if (!fileExist) {
            handle_ << "[\n";
        } else {
            prepareAppendOnExistingRecord();
        }
    }

    void pushRecord(unsigned vKey, bool isKeyUp) noexcept {
		DateTime dt;

        if (!handle_.is_open()) {
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
        if (!handle_.is_open()) {
            return;
        }

        handle_ << "\n]\n" << std::flush;
        handle_.close();

		DateTime dt;

        dt.now();
        logger_.write(dt.datetime(), "Info", "Storage file closed successfully");

		status_ = Status::Idle;
    }

    void rolloverIfNewDay() {
        DateTime dt;
        dt.now();

        if (!openedAt_.isSameDay(dt)) {
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
    std::fstream handle_;
    DateTime openedAt_;
    Logger logger_;
    bool isFirstRecord_;
	Status status_;

    [[nodiscard]] inline bool hasContent(const std::string& targetFile) const noexcept {
        std::ifstream test(targetFile);
        return test.is_open() && test.peek() != std::ifstream::traits_type::eof();
    }

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