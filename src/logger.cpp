#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <logger.hpp>

#if TYPECOUNTER_VERSION >= 2026000100ull

static constexpr const char* STORED_AT = "logs";
static constexpr const char* OUTPUT_EXT = ".log";
static constexpr std::ios::openmode FS_DEFAULT_MODE = std::ios::app;
static constexpr uint8_t MAX_LENGTH_FILENAME = 16;

namespace zuu {

namespace fs = std::filesystem;

static inline fs::path LOGS_DIR = fs::absolute(fs::current_path() / STORED_AT);

Logger::Logger()
 : opened_at_(Datetime::Now()) {
	if (active_instance_) {
		return;
	}

	active_instance_ = this;
	initialize();
}

Logger::~Logger() {
	if (active_instance_ == this) {
		if (handle_.is_open()) {
			handle_.close();
		}
	}

	active_instance_ = nullptr;
}

void Logger::info(const std::string& msg) {
	try_rollback();
	auto dt = Datetime::Now();
	handle_ << dt.get_datetime_str() << " [INFO] " << msg << '\n' << std::flush;
}

void Logger::warning(const std::string& msg) {
	try_rollback();
	auto dt = Datetime::Now();
	handle_ << dt.get_datetime_str() << " [WARNING] " << msg << '\n' << std::flush;
}

void Logger::error(const std::string& msg) {
	try_rollback();
	auto dt = Datetime::Now();
	handle_ << dt.get_datetime_str() << " [ERROR] " << msg << '\n' << std::flush;
}

void Logger::critical(const std::string& msg) {
	try_rollback();
	auto dt = Datetime::Now();
	handle_ << dt.get_datetime_str() << " [CRITICAL] " << msg << '\n' << std::flush;
}

Logger* Logger::GetInstance() noexcept {
	return active_instance_;
}

void Logger::initialize() {
	std::string fullname;

	prepare_directory();

	fullname.reserve(MAX_LENGTH_FILENAME);
	fullname += active_instance_->opened_at_.get_date_str();
	fullname += OUTPUT_EXT;

	active_instance_->try_open(fullname.c_str());
}

// === Private API

void Logger::try_rollback() {
	auto dt = Datetime::Now();
	
	if (opened_at_.get_date_str() != dt.get_date_str()) {
		std::string fullname;

		fullname.reserve(MAX_LENGTH_FILENAME);
		fullname += dt.get_date_str();
		fullname += OUTPUT_EXT;

		if (handle_.is_open()) {
			handle_.close();
		}
		
		if (!try_open(fullname.c_str())) {
			auto dt = Datetime::Now();

			std::cerr << dt.get_datetime_str() << " [CRITICAL] Failed to rollback log!\n";
			std::abort();
		}

		opened_at_ = dt;
	}
}

bool Logger::try_open(const char* filename) {
    handle_.open(LOGS_DIR / filename, FS_DEFAULT_MODE);

    if (!handle_.is_open()) {
        auto dt = Datetime::Now();
        std::cerr << dt.get_datetime_str() << " [CRITICAL] Failed to initializing logger!\n";
        return false;
    }

    return true;
}

void Logger::prepare_directory() noexcept {
    std::error_code errc;

    if (!fs::exists(LOGS_DIR, errc)) {
        if (errc) {
            auto dt = Datetime::Now();
            std::cerr << dt.get_datetime_str() << " [CRITICAL] " << errc.message() << "\n";
            std::abort();
        }

        fs::create_directories(LOGS_DIR, errc);
    }

    if (errc) {
        auto dt = Datetime::Now();
        std::cerr << dt.get_datetime_str() << " [CRITICAL] " << errc.message() << "\n";
        std::abort();
    }
}

} // namespace zuu

#endif // TYPECOUNTER_VERSION >= 2026000100ull