include("${CMAKE_SOURCE_DIR}/cmake/packages/google-test.cmake")

add_executable(
	datetime_test
		${CMAKE_CURRENT_SOURCE_DIR}/datetime_test.cpp
)

target_include_directories(
	datetime_test
		PRIVATE
			"${CMAKE_SOURCE_DIR}/include"
)

target_link_libraries(
	datetime_test
		PRIVATE
			GTest::gtest_main
			${LIBRARY_OUTPUT_NAME}
)

add_executable(
	logger_test
		${CMAKE_CURRENT_SOURCE_DIR}/logger_test.cpp
)

target_include_directories(
	logger_test
		PRIVATE
			"${CMAKE_SOURCE_DIR}/include"
)

target_link_libraries(
	logger_test
		PRIVATE
			GTest::gtest_main
			${LIBRARY_OUTPUT_NAME}
)

gtest_discover_tests(datetime_test)
gtest_discover_tests(logger_test)