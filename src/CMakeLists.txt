add_executable(collector
	${CMAKE_CURRENT_SOURCE_DIR}/app.cpp
)

target_include_directories(collector 
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
)

if (WIN32)
    target_link_libraries(collector PRIVATE user32)
endif()