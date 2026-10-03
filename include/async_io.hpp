#pragma once

#include <version.hpp>

#if TYPECOUNTER_VERSION >= 2026000100ull

#include <cstdint>
#include <functional>
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

    void submit_write_job(const std::string& filepath, const void* data_ptr, size_t byte_size);
    void submit_read_job(const std::string& filepath, std::function<void(std::vector<uint8_t>)> on_complete);

private:
	struct WriterJob {
		std::string filepath;
		std::vector<uint8_t> raw_data;
	};

	struct ReaderJob {
		std::string filepath;
		std::function<void(std::vector<uint8_t>)> on_complete;
	};

	std::atomic<bool> is_running_{true};

	std::thread writer_thread_;
	std::thread reader_thread_;
    std::vector<WriterJob> write_jobs_;
    std::vector<ReaderJob> read_jobs_;
	std::condition_variable cv_write_;
	std::condition_variable cv_read_;
	std::mutex mtx_write_;
	std::mutex mtx_read_;

    AsyncIO();
    void writer_loop();
    void reader_loop();
};

} // namespace zuu

#endif