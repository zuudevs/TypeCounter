#pragma once

#include <datetime.hpp>

#if TYPECOUNTER_VERSION >= 2026000100ull

#include <cstdint>
#include <string>
#include <mutex>

namespace zuu {

class Logger {
public:
	[[nodiscard]] static Logger& GetInstance() noexcept;

    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;
	
    ~Logger();

    void info(const std::string& msg);
    void warning(const std::string& msg);
    void error(const std::string& msg);
    void critical(const std::string& msg);

private:    
    std::mutex mtx_;
    std::string text_buffer_;

	Logger();
    
    void prepare_directory() const;
    [[nodiscard]] std::string get_current_filepath() const;
    
    void push_log(uint8_t severity, const std::string& msg, bool force_flush);
    void flush_to_worker();
};

} // namespace zuu

#endif // TYPECOUNTER_VERSION >= 2026000100ull