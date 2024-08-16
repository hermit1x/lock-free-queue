cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 11)

set(BENCHMARK_DOWNLOAD_DEPENDENCIES 1)
add_subdirectory(benchmark)



add_executable(que_benchmark que_benchmark.cpp)
target_link_libraries(que_benchmark benchmark::benchmark)