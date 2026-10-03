#pragma once

#include <version.hpp>

#if TYPECOUNTER_VERSION >= 2026000100ull

#include <vector>
#include <string>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <cstring>

namespace zuu {

class AsyncIO {
public:
    [[nodiscard]] static AsyncIO& GetInstance() noexcept;

    AsyncIO(const AsyncIO&) = delete;
    AsyncIO& operator=(const AsyncIO&) = delete;

    ~AsyncIO();

    void submit_job(const std::string& filepath, const void* data_ptr, size_t byte_size);

private:
	struct IOJob {
		std::string filepath;
		std::vector<uint8_t> raw_data;
	};

	std::atomic<bool> is_running_{true};
	std::thread worker_thread_;
    std::vector<IOJob> jobs_;
	std::condition_variable cv_;
	std::mutex mtx_;

    AsyncIO();
    void worker_loop();
};

} // namespace zuu

#endif