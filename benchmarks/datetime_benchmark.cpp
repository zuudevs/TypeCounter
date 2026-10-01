#include <datetime.hpp>
#include <benchmark/benchmark.h>

static inline constexpr auto first_date_in_oct_2026 = 1790787600000ull;

static void Datetime_Now(benchmark::State& state) {
	for (auto _ : state) {
		auto dt = zuu::Datetime::Now();
		benchmark::DoNotOptimize(dt);
		benchmark::ClobberMemory();
	}
}

static void Datetime_FromEpoch(benchmark::State& state) {
	for (auto _ : state) {
		auto dt = zuu::Datetime::FromEpoch(first_date_in_oct_2026);
		benchmark::DoNotOptimize(dt);
		benchmark::ClobberMemory();
	}
}

static void Datetime_get_datetime_str(benchmark::State& state) {
	auto dt = zuu::Datetime::FromEpoch(first_date_in_oct_2026);
	for (auto _ : state) {
		auto datetime_str = dt.get_datetime_str();
		benchmark::DoNotOptimize(datetime_str);
		benchmark::ClobberMemory();
	}
}

static void Datetime_get_date_str(benchmark::State& state) {
	auto dt = zuu::Datetime::FromEpoch(first_date_in_oct_2026);
	for (auto _ : state) {
		auto date_str = dt.get_date_str();
		benchmark::DoNotOptimize(date_str);
		benchmark::ClobberMemory();
	}
}

static void Datetime_get_timestamp(benchmark::State& state) {
	auto dt = zuu::Datetime::FromEpoch(first_date_in_oct_2026);
	for (auto _ : state) {
		auto timestamp = dt.get_timestamp();
		benchmark::DoNotOptimize(timestamp);
		benchmark::ClobberMemory();
	}
}

static void Datetime_get_epoch(benchmark::State& state) {
	auto dt = zuu::Datetime::FromEpoch(first_date_in_oct_2026);
	for (auto _ : state) {
		auto epoch = dt.get_epoch();
		benchmark::DoNotOptimize(epoch);
		benchmark::ClobberMemory();
	}
}

BENCHMARK(Datetime_Now)->MinTime(2.0)->Unit(benchmark::kNanosecond);
BENCHMARK(Datetime_FromEpoch)->MinTime(2.0)->Unit(benchmark::kNanosecond);
BENCHMARK(Datetime_get_datetime_str)->MinTime(2.0)->Unit(benchmark::kNanosecond);
BENCHMARK(Datetime_get_date_str)->MinTime(2.0)->Unit(benchmark::kNanosecond);
BENCHMARK(Datetime_get_timestamp)->MinTime(2.0)->Unit(benchmark::kNanosecond);
BENCHMARK(Datetime_get_epoch)->MinTime(2.0)->Unit(benchmark::kNanosecond);