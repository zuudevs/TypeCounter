#pragma once

#include <async_io.hpp>
#include <logger.hpp>
#include <datetime.hpp>

#if TYPECOUNTER_VERSION >= 2026000100ull

#include <fstream>
#include <cstring>
#include <cstdint>
#include <filesystem>
#include <vector>

#ifndef ZUU_ABRV_GUARD
#define ZUU_ABRV_GUARD
#endif

namespace zuu {

#ifdef ZUU_ABRV_GUARD
namespace fs = std::filesystem;
#endif

#pragma pack(push, 1)
struct StorageInfo {
    unsigned long long records{};
	unsigned char version{TYPECOUNTER_VERSION_MAJOR};
	unsigned char component{};
};
#pragma pack(pop)

template <typename Derived_, typename RecordType_>
class Reporter {
public:
    using Path = fs::path;
	using Derived = Derived_;
	using RecordType = RecordType_;

    static constexpr size_t MAX_QUEUE_SIZE = 128;

    explicit Reporter() noexcept {
		auto& _ = AsyncIO::GetInstance();

        queue_.reserve(MAX_QUEUE_SIZE);
    }

    ~Reporter() {
        std::scoped_lock lock(mtx_);
        flush_to_worker();
    }

    inline void push_record(RecordType record) {
        bool should_flush = false;
        {
            std::scoped_lock lock(mtx_);
            queue_.push_back(record);
            
            if (queue_.size() >= MAX_QUEUE_SIZE) {
                should_flush = true;
            }
        }

        if (should_flush) {
            flush_to_worker();
        }
    }

	[[nodiscard]] inline Path prepare_directory(Path path) const {
        Path absPath = fs::absolute(fs::current_path() / path);
        if (!fs::exists(absPath)) {
            fs::create_directories(absPath);
        }
        return absPath;
    }

    [[nodiscard]] inline bool ensure_header(Path abs_path, uint64_t& out_records) noexcept {
        std::fstream handle(abs_path, std::ios::in | std::ios::out | std::ios::binary);
        
        if (!handle.is_open()) {
            std::ofstream creator(abs_path, std::ios::binary);
            StorageInfo info{};
            info.component = static_cast<unsigned char>(Derived::COMPONENT_ID);
            creator.write(reinterpret_cast<const char*>(&info), sizeof(info));
            creator.close();
            
            out_records = 0;
            return true;
        }

        handle.seekg(0, std::ios::end);
        const auto file_size = handle.tellg();
        constexpr std::streamoff header_size = sizeof(StorageInfo);
        const auto record_size = static_cast<std::streamoff>(sizeof(RecordType));

        if (file_size < header_size) {
            return false;
        }

        const auto data_bytes = file_size - header_size;
        out_records = static_cast<uint64_t>(data_bytes / record_size);
        
        return true;
    }

protected:
	uint64_t total_records_{0};
    Datetime opened_at_;
    std::vector<RecordType> queue_;
    std::mutex mtx_;

    void flush_to_worker() {
        if (queue_.empty()) {
			return;
		}

        std::string target_file = derived().provide_name().string();

        AsyncIO::GetInstance().submit_write_job(
            target_file,
            queue_.data(),
            queue_.size() * sizeof(RecordType)
        );

        queue_.clear();
    }

	inline constexpr Derived& derived() noexcept { return *static_cast<Derived*>(this); }
    inline constexpr const Derived& derived() const noexcept { return *static_cast<const Derived*>(this); }

	// static_assert(
	// 	std::is_same_v<decltype(std::declval<Reporter>().derived().provide_name()), Path>, 
	// 	R"(provide_name() must in "Path" type)"
	// );

	static_assert(
		std::is_integral_v<decltype(Reporter::Derived::COMPONENT_ID)>, 
		R"(COMPONENT_ID must in integer type)"
	);
};

} // namespace zuu

#ifdef ZUU_ABRV_GUARD
#undef ZUU_ABRV_GUARD
#endif

#endif