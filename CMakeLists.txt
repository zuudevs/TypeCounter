cmake_minimum_required(VERSION 4.4.0)
project(
    TypeCounter
    VERSION 0.1.0
    DESCRIPTION "TypeCounter is an automatic typing counter app with a handy screen widget"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LIBRARY_OUTPUT_NAME "typecounter_core")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

include("${CMAKE_SOURCE_DIR}/cmake/tools/git.cmake")

add_subdirectory("${CMAKE_SOURCE_DIR}/cmake/templates")
add_subdirectory("${CMAKE_SOURCE_DIR}/src")
add_subdirectory("${CMAKE_SOURCE_DIR}/tests")
add_subdirectory("${CMAKE_SOURCE_DIR}/benchmarks")

message("")
message("Project Name       : ${PROJECT_NAME}")
message("Project Version    : ${PROJECT_VERSION}")
message("Build Type         : ${CMAKE_BUILD_TYPE}")
message("C++ Standard       : ${CMAKE_CXX_STANDARD}")
message("C++ Compiler       : ${CMAKE_CXX_COMPILER}")
message("CMake Generator    : ${CMAKE_GENERATOR}")
message("Git Commit Hash    : ${GIT_COMMIT_HASH}")
message("")