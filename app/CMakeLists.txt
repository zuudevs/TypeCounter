include("${CMAKE_SOURCE_DIR}/cmake/modules/optimizations.cmake")

add_executable(
	"TypeCounter"
		"${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/reporter/keyboard_reporter.cpp"
)

target_include_directories(
	"TypeCounter"
		PUBLIC
			${CMAKE_SOURCE_DIR}/include
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
	"TypeCounter"
		PRIVATE
			${LIBRARY_OUTPUT_NAME}
)

add_release_optimizations_target("TypeCounter")