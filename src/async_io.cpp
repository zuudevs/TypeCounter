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
	cv_write_.notify_one();
	cv_read_.notify_one();

	if (writer_thread_.joinable()) {
		writer_thread_.join();
	}

	if (reader_thread_.joinable()) {
		reader_thread_.join();
	}
}

void AsyncIO::submit_write_job(const std::string& filepath, const void* data_ptr, size_t byte_size) {
	if (byte_size == 0 || data_ptr == nullptr) {
		return;
	}

	std::vector<uint8_t> buffer(byte_size);
	std::memcpy(buffer.data(), data_ptr, byte_size);

	{
		std::scoped_lock lock(mtx_write_);
		write_jobs_.push_back({
			filepath, 
			std::move(buffer)
		});
	}
	cv_write_.notify_one();
}

void AsyncIO::submit_read_job(const std::string& filepath, std::function<void(std::vector<uint8_t>)> on_complete) {
    {
        std::scoped_lock lock(mtx_read_);
        read_jobs_.push_back({
			filepath, 
			std::move(on_complete)
		});
    }
    cv_read_.notify_one();
}

AsyncIO::AsyncIO() {
	write_jobs_.reserve(128); 
	read_jobs_.reserve(32); 

	writer_thread_ = std::thread(&AsyncIO::writer_loop, this);
	reader_thread_ = std::thread(&AsyncIO::reader_loop, this);
}

void AsyncIO::writer_loop() {
	std::vector<WriterJob> local_jobs;
	local_jobs.reserve(128);

	std::unordered_map<std::string, std::ofstream> active_files;

	while (true) {
		{
			std::unique_lock<std::mutex> lock(mtx_write_);
			cv_write_.wait(lock, [this]() {
				return !is_running_.load() || !write_jobs_.empty();
			});
			local_jobs.swap(write_jobs_);
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

void AsyncIO::reader_loop() {
    std::vector<ReaderJob> local_jobs;
    local_jobs.reserve(32);

    while (true) {
        {
            std::unique_lock<std::mutex> lock(mtx_read_);
            cv_read_.wait(lock, [this]() {
                return !is_running_.load() || !read_jobs_.empty();
            });
            local_jobs.swap(read_jobs_);
        }

        if (!local_jobs.empty()) {
            for (auto& job : local_jobs) {
                std::ifstream file(job.filepath, std::ios::binary | std::ios::ate);
                
                if (file.is_open()) {
                    std::streamsize size = file.tellg();
                    file.seekg(0, std::ios::beg);

                    std::vector<uint8_t> buffer(size);
                    
                    if (file.read(reinterpret_cast<char*>(buffer.data()), size)) {
                        if (job.on_complete) {
							job.on_complete(std::move(buffer));
						}
                    } else {
                        std::cerr << "[Critical] AsyncIO: Failed to read bytes from: " << job.filepath << "\n";
                        if (job.on_complete) {
							job.on_complete({});
						}
                    }
                } else {
                    std::cerr << "[Citical] AsyncIO: File not found or locked by OS: " << job.filepath << "\n";
                    if (job.on_complete) {
						job.on_complete({});
					}
                }
            }
            local_jobs.clear();
        }

        if (!is_running_.load()) {
			break;
		}
    }
}

} // namespace zuu

#endif