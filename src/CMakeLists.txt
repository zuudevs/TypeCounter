include("${CMAKE_SOURCE_DIR}/cmake/modules/optimizations.cmake")

add_library(
	${LIBRARY_OUTPUT_NAME} 
		SHARED
			${CMAKE_CURRENT_SOURCE_DIR}/datetime.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/async_io.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/logger.cpp
)

target_include_directories(
	${LIBRARY_OUTPUT_NAME}
		PRIVATE
			"${CMAKE_SOURCE_DIR}/include"
)

add_release_optimizations_target(${LIBRARY_OUTPUT_NAME})