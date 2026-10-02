include("${CMAKE_SOURCE_DIR}/cmake/modules/optimizations.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/packages/google-benchmark.cmake")

add_executable(
	datetime_benchmark
		${CMAKE_CURRENT_SOURCE_DIR}/datetime_benchmark.cpp
)

target_include_directories(
	datetime_benchmark
		PRIVATE
			"${CMAKE_SOURCE_DIR}/include"
)

target_link_libraries(
	datetime_benchmark
		PRIVATE
			benchmark::benchmark_main
			${LIBRARY_OUTPUT_NAME}
)

add_release_optimizations_target("datetime_benchmark")

add_executable(
	logger_benchmark
		${CMAKE_CURRENT_SOURCE_DIR}/logger_benchmark.cpp
)

target_include_directories(
	logger_benchmark
		PRIVATE
			"${CMAKE_SOURCE_DIR}/include"
)

target_link_libraries(
	logger_benchmark
		PRIVATE
			benchmark::benchmark_main
			${LIBRARY_OUTPUT_NAME}
)

add_release_optimizations_target("logger_benchmark")