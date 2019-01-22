cmake_minimum_required(VERSION 3.8)
project(varvector LANGUAGES CXX)

# Compile options
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(optimization_flag -O0)
else()
    set(optimization_flag -Ofast)
endif()

set(CMAKE_CXX_COMPILER clang++)

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

# Build
## Benchmark binary: `bench`
if (NOT benchmark_FOUND)
    message(WARNING "benchmark not present, cannot build bench binary")
else()
    add_executable(bench
        ${bench_dir}/varvector.cpp
    )
    target_compile_features(bench PUBLIC cxx_std_17)
    target_link_libraries(bench benchmark)
    target_compile_options(bench PUBLIC ${optimization_flag})
    target_include_directories(bench
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/lib
            ${CMAKE_CURRENT_SOURCE_DIR}/bench)
endif()

## Varvector test binary: `varvector_test`
add_executable(varvector_test
    ${test_dir}/varvector_test.cpp
)
target_compile_features(varvector_test PUBLIC cxx_std_17)
target_compile_options(bench PUBLIC ${optimization_flag})
target_include_directories(varvector_test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/lib)

## Compact_idx test binary: `compact_idx_test`
add_executable(compact_idx_test
    ${test_dir}/compact_idx_test.cpp
)
target_compile_features(compact_idx_test PUBLIC cxx_std_17)
target_compile_options(bench PUBLIC ${optimization_flag})
target_include_directories(compact_idx_test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/lib)