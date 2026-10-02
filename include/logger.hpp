#pragma once

#include <version.hpp> 

#if TYPECOUNTER_VERSION >= 2026000100ull

#include <cstdint>
#include <vector>
#include <string>
#include <datetime.hpp>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>

namespace zuu {

class Logger {
public:
    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;
    ~Logger();

    void info(std::string msg);
    void warning(std::string msg);
    void error(std::string msg);
    void critical(std::string msg);
    [[nodiscard]] static Logger* GetInstance() noexcept;

private:
    enum Severity : uint8_t {
        Info,
        Warning,
        Error,
        Critical
    };

    static inline Logger* active_instance_ = nullptr;

    std::atomic<bool> is_running_{true};
    Datetime opened_at_; 
    
    std::thread worker_;
    
    // SoA (Structure of Arrays)
    std::vector<Severity> queue_severity_;
    std::vector<std::string> queue_msg_;
    
    std::condition_variable cv_;
    std::mutex mtx_;

    [[nodiscard]] static inline constexpr const char* translate_severity(Severity severity) noexcept {
        switch (severity) {
            case Severity::Info: return "[INFO] ";
            case Severity::Warning: return "[WARNING] ";
            case Severity::Error: return "[ERROR] ";
            case Severity::Critical: return "[CRITICAL] ";
            default: return "[UNKNOWN] ";
        }
    }

    void prepare_directory() noexcept;
    void push_log(Severity severity, std::string msg, bool force_flush);
    void background_write();
};

} // namespace zuu

#endif // TYPECOUNTER_VERSION >= 2026000100ull