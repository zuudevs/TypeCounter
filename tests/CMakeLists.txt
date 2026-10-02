function(RunAndExportTestReport name executable_name)
	set(FILENAME_POSTFIX_ "")
	if(NOT GIT_COMMIT_HASH STREQUAL "unknown_hash")
		set(FILENAME_POSTFIX_ "_${GIT_COMMIT_HASH}")
	endif()
	
    set(RESULT_DIR "${CMAKE_SOURCE_DIR}/out/results/test")
    add_custom_target(run_and_export_${name}_test_report
		COMMAND ${CMAKE_COMMAND} -E make_directory "${RESULT_DIR}"
        COMMAND $<TARGET_FILE:${executable_name}> 
            --gtest_output=json:${RESULT_DIR}/${executable_name}${FILENAME_POSTFIX_}.json
        DEPENDS ${executable_name}
        COMMENT "Run & Export Report for ${name}"
    )   
endfunction()

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

gtest_discover_tests(datetime_test)
RunAndExportTestReport(datetime datetime_test)

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

gtest_discover_tests(logger_test)
RunAndExportTestReport(logger logger_test)