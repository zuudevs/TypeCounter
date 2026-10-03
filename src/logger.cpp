#include <logger.hpp>
#include <async_io.hpp>

#if TYPECOUNTER_VERSION >= 2026000100ull

#include <iostream>
#include <filesystem>

namespace zuu {

namespace fs = std::filesystem;

static constexpr const char* STORED_AT = "logs";
static constexpr const char* OUTPUT_EXT = ".log";
static constexpr size_t MAX_BUFFER_SIZE = 4096; 

enum Severity : uint8_t {
	Info,
	Warning,
	Error,
	Critical
};

[[nodiscard]] static inline constexpr const char* translate_severity(Severity severity) noexcept {
    switch (severity) {
        case Severity::Info: return "[INFO] ";
        case Severity::Warning: return "[WARNING] ";
        case Severity::Error: return "[ERROR] ";
        case Severity::Critical: return "[CRITICAL] ";
        default: return "[UNKNOWN] ";
    }
}

Logger& Logger::GetInstance() noexcept {
	static Logger instance;
    return instance;
}

Logger::~Logger() {
    std::scoped_lock lock(mtx_);
    flush_to_worker();
}

Logger::Logger() {
	auto& _ = AsyncIO::GetInstance();
    prepare_directory();
    text_buffer_.reserve(MAX_BUFFER_SIZE * 2); 
}

void Logger::prepare_directory() const {
    std::error_code errc;
    fs::path dir = fs::absolute(fs::current_path() / STORED_AT);

    if (!fs::exists(dir, errc)) {
        fs::create_directories(dir, errc);
    }

    if (errc) {

        std::cerr << "[CRITICAL] Gagal membuat direktori logs: " << errc.message() << "\n";
        std::abort();
    }
}

std::string Logger::get_current_filepath() const {
    fs::path dir = fs::absolute(fs::current_path() / STORED_AT);
    std::string filename = Datetime::Now().get_date_str() + OUTPUT_EXT;
    return (dir / filename).string();
}

void Logger::info(const std::string& msg) { push_log(Severity::Info, msg, false); }
void Logger::warning(const std::string& msg) { push_log(Severity::Warning, msg, false); }
void Logger::error(const std::string& msg) { push_log(Severity::Error, msg, true); }
void Logger::critical(const std::string& msg) { push_log(Severity::Critical, msg, true); }

void Logger::push_log(uint8_t severity, const std::string& msg, bool force_flush) {
    bool should_flush = force_flush;
    
    {
        std::scoped_lock lock(mtx_);
        
        text_buffer_ += Datetime::Now().get_datetime_str();
        text_buffer_ += " ";
        text_buffer_ += translate_severity(static_cast<Severity>(severity));
        text_buffer_ += msg;
        text_buffer_ += '\n';

        if (text_buffer_.size() >= MAX_BUFFER_SIZE) {
            should_flush = true;
        }

        if (should_flush) {
            flush_to_worker();
        }
    }
}

void Logger::flush_to_worker() {
    if (text_buffer_.empty()) {
		return;
	}

    AsyncIO::GetInstance().submit_job(
        get_current_filepath(),
        text_buffer_.data(),
        text_buffer_.size()
    );

    text_buffer_.clear();
}

} // namespace zuu

#endif // TYPECOUNTER_VERSION >= 2026000100ull