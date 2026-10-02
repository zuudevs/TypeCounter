#include <logger.hpp>

#if TYPECOUNTER_VERSION >= 2026000100ull

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <fstream>

static constexpr const char* STORED_AT = "logs";
static constexpr const char* OUTPUT_EXT = ".log";
static constexpr std::ios::openmode FS_DEFAULT_MODE = std::ios::app;
static constexpr uint8_t MAX_LENGTH_FILENAME = 16;
static constexpr uint64_t MAX_QUEUE_SIZE = 16;

namespace zuu {

namespace fs = std::filesystem;

static inline fs::path LOGS_DIR = fs::absolute(fs::current_path() / STORED_AT);

Logger::Logger() {
    if (active_instance_) {
        return;
    }

    active_instance_ = this;
    
    prepare_directory();
    
    worker_ = std::thread(&Logger::background_write, this);
}

Logger::~Logger() {
    if (active_instance_ == this) {
        is_running_.store(false);
        cv_.notify_one();
        if (worker_.joinable()) worker_.join();
        active_instance_ = nullptr;
    }
}

void Logger::info(std::string msg) { push_log(Severity::Info, std::move(msg), false); }
void Logger::warning(std::string msg) { push_log(Severity::Warning, std::move(msg), false); }
void Logger::error(std::string msg) { push_log(Severity::Error, std::move(msg), true); }
void Logger::critical(std::string msg) { push_log(Severity::Critical, std::move(msg), true); }

Logger* Logger::GetInstance() noexcept {
    return active_instance_;
}

// === Private API ===

void Logger::push_log(Severity severity, std::string msg, bool force_flush) {
    bool should_flush = force_flush;
    {
        std::scoped_lock lock(mtx_);
        queue_severity_.push_back(severity);
        queue_msg_.push_back(std::move(msg));

        if (queue_msg_.size() >= MAX_QUEUE_SIZE) {
            should_flush = true;
        }
    }

    if (should_flush) {
		cv_.notify_one();
	}
}

void Logger::prepare_directory() noexcept {
    std::error_code errc;

    if (!fs::exists(LOGS_DIR, errc)) {
        fs::create_directories(LOGS_DIR, errc);
    }

    if (errc) {
        auto dt = Datetime::Now();
        std::cerr << dt.get_datetime_str() << " [CRITICAL] " << errc.message() << "\n";
        std::abort();
    }
}

void Logger::background_write() {
	std::ofstream handle_;
    std::vector<Severity> local_sev;
    std::vector<std::string> local_msg;
    local_sev.reserve(MAX_QUEUE_SIZE);
    local_msg.reserve(MAX_QUEUE_SIZE);

    auto check_and_rollback_log = [&]() {
        auto current_dt = Datetime::Now();
        
        if (!handle_.is_open() || opened_at_.get_date_str() != current_dt.get_date_str()) {
            if (handle_.is_open()) handle_.close();
            
            opened_at_ = current_dt;
            std::string filename;
            filename.reserve(MAX_LENGTH_FILENAME);
            filename += opened_at_.get_date_str();
            filename += OUTPUT_EXT;

            handle_.open(LOGS_DIR / filename, FS_DEFAULT_MODE);
            
            if (!handle_.is_open()) {
                std::cerr << current_dt.get_datetime_str() << " [CRITICAL] Failed to open/rollback log!\n";
                std::abort();
            }
        }
    };

    check_and_rollback_log();

    while (true) {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [&](){
                return !is_running_.load() || !queue_msg_.empty();
            });

            local_sev.swap(queue_severity_);
            local_msg.swap(queue_msg_);
        }

        if (!local_msg.empty()) {
            check_and_rollback_log();

            for (size_t i = 0; i < local_msg.size(); ++i) {
                handle_ << Datetime::Now().get_datetime_str() << " " 
                        << translate_severity(local_sev[i]) 
                        << local_msg[i] << '\n';
            }
            handle_.flush();

            local_sev.clear();
            local_msg.clear();
        }

        if (!is_running_.load()) {
			break;
		}
    }
}

} // namespace zuu

#endif // TYPECOUNTER_VERSION >= 2026000100ull