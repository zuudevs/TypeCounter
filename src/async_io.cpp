#include <async_io.hpp>

#if TYPECOUNTER_VERSION >= 2026000100ull

#include <fstream>
#include <unordered_map>
#include <iostream>

namespace zuu {

AsyncIO& AsyncIO::GetInstance() noexcept {
	static AsyncIO instance;
	return instance;
}

AsyncIO::~AsyncIO() {
	is_running_.store(false);
	cv_.notify_one();
	if (worker_thread_.joinable()) {
		worker_thread_.join();
	}
}

void AsyncIO::submit_job(const std::string& filepath, const void* data_ptr, size_t byte_size) {
	if (byte_size == 0 || data_ptr == nullptr) {
		return;
	}

	std::vector<uint8_t> buffer(byte_size);
	std::memcpy(buffer.data(), data_ptr, byte_size);

	{
		std::scoped_lock lock(mtx_);
		jobs_.push_back({
			filepath, 
			std::move(buffer)
		});
	}
	cv_.notify_one();
}

AsyncIO::AsyncIO() {
	jobs_.reserve(128); 
	worker_thread_ = std::thread(&AsyncIO::worker_loop, this);
}

void AsyncIO::worker_loop() {
	std::vector<IOJob> local_jobs;
	local_jobs.reserve(128);

	std::unordered_map<std::string, std::ofstream> active_files;

	while (true) {
		{
			std::unique_lock<std::mutex> lock(mtx_);
			cv_.wait(lock, [this]() {
				return !is_running_.load() || !jobs_.empty();
			});
			local_jobs.swap(jobs_);
		}

		if (!local_jobs.empty()) {
			for (auto& job : local_jobs) {
				
				auto it = active_files.find(job.filepath);
				
				if (it == active_files.end()) {
					active_files[job.filepath] = std::ofstream(job.filepath, std::ios::app | std::ios::binary);
					it = active_files.find(job.filepath);
				}

				if (it->second.is_open()) {
					it->second.write(reinterpret_cast<const char*>(job.raw_data.data()), job.raw_data.size());
				} else {
					std::cerr << "[Critical] Failed to open: " << job.filepath << "\n";
				}
			}

			for (auto& [filepath, stream] : active_files) {
				if (stream.is_open()) stream.flush();
			}

			local_jobs.clear();
		}

		if (!is_running_.load()) {
			for (auto& [filepath, stream] : active_files) {
				if (stream.is_open()) {
					stream.close();
				}
			}
			break;
		}
	}
}

} // namespace zuu

#endif