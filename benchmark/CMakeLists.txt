set(BINARY ${CMAKE_PROJECT_NAME}_bench)
set(CMAKE_CXX_STANDARD 20)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG master
        GIT_PROGRESS TRUE
)

FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG master
        GIT_PROGRESS TRUE
)

FetchContent_GetProperties(googletest)
if (NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
endif()

FetchContent_GetProperties(benchmark)
if (NOT benchmark_POPULATED)
    FetchContent_Populate(benchmark)
endif()

add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
add_subdirectory(${benchmark_SOURCE_DIR} ${benchmark_BINARY_DIR})


set(SOURCES stats_benchmarks.cpp)
add_executable(${BINARY} ${SOURCES})
target_link_libraries(${BINARY} PUBLIC statsdb benchmark::benchmark)
