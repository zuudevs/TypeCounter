#pragma once

#include <version.hpp>

#if TYPECOUNTER_VERSION >= 2026000100ull

#include <fstream>
#include <datetime.hpp>

namespace zuu {

class Logger {
public:
	Logger();
	Logger(const Logger&) = delete;
	Logger(Logger&&) = delete;
	Logger& operator=(const Logger&) = delete;
	Logger& operator=(Logger&&) = delete;
	~Logger();

	void info(const std::string& msg);
	void warning(const std::string& msg);
	void error(const std::string& msg);
	void critical(const std::string& msg);
	[[nodiscard]] static Logger* GetInstance() noexcept;

private:
	static inline Logger* active_instance_ = nullptr;

	Datetime opened_at_;
	std::ofstream handle_;

	void initialize();
	void try_rollback();
	bool try_open(const char* filename);
	void prepare_directory() noexcept;
};

} // namespace zuu

#endif // TYPECOUNTER_VERSION >= 2026000100ull