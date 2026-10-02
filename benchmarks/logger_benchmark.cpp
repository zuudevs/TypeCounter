#include <benchmark/benchmark.h>
#include <logger.hpp>

inline std::string create_payload(std::size_t len) {
	std::string res;
	res.reserve(len);

	constexpr auto RANGE = 'Z' - 'A' + 1;
	for(auto i  = 0; i < len; i++) {
		res += 'A' + (i % RANGE);
	}
	
	return res;
}

static void Logger_Initializing(benchmark::State& state) {
	for(auto _ : state) {
		auto log = zuu::Logger();
		benchmark::DoNotOptimize(log);
		benchmark::ClobberMemory();
	}
}

static void Logger_Payload16(benchmark::State& state) {
	std::string payload = create_payload(16);
	auto log = zuu::Logger();
	for(auto _ : state) {
		log.info(payload);
		benchmark::ClobberMemory();
	}
}

static void Logger_Payload32(benchmark::State& state) {
	std::string payload = create_payload(32);
	auto log = zuu::Logger();
	for(auto _ : state) {
		log.info(payload);
		benchmark::ClobberMemory();
	}
}

static void Logger_Payload64(benchmark::State& state) {
	std::string payload = create_payload(64);
	auto log = zuu::Logger();
	for(auto _ : state) {
		log.info(payload);
		benchmark::ClobberMemory();
	}
}

BENCHMARK(Logger_Initializing)->MinTime(2.0)->Unit(benchmark::kNanosecond);;
BENCHMARK(Logger_Payload16)->MinTime(2.0)->Unit(benchmark::kNanosecond);;
BENCHMARK(Logger_Payload32)->MinTime(2.0)->Unit(benchmark::kNanosecond);;
BENCHMARK(Logger_Payload64)->MinTime(2.0)->Unit(benchmark::kNanosecond);;