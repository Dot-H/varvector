cmake_minimum_required(VERSION 3.8)

# Compile options
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -Ofast -Weverything)

# Force colored output for Ninja builds.
if (${CMAKE_GENERATOR} STREQUAL Ninja)
    add_compile_options(-fcolor-diagnostics)
endif()

# Package research
find_package(benchmark CONFIG)

# Path variables
set (bench_dir ${CMAKE_CURRENT_SOURCE_DIR}/bench)
set (test_dir ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Build
## Benchmark binary: `bench`
if (NOT benchmark_FOUND)
    message(WARNING "benchmark not present, cannot build bench binary")
else()
    add_executable(bench
        ${bench_dir}/vector_variant.cpp
    )
    target_link_libraries(bench benchmark)
endif()
## Test binary: `test`
add_executable(test
    ${test_dir}/test.cpp
)