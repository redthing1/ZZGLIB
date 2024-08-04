cmake_minimum_required(VERSION 3.10)
project(ZZG_Hash_Benchmark)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set optimization flags
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Add source files
set(COMMON_SOURCES
    ZZG_Mem.cpp
    ZZG_Sync.cpp
)

# Create executable for the simple benchmark
add_executable(benchmark_simple 
    benchmark_simple.cpp
    ${COMMON_SOURCES}
)

# Create executable for the multi-threaded benchmark
add_executable(benchmark_mthread
    benchmark_mthread.cpp
    ${COMMON_SOURCES}
)

# Find and link pthread for multi-threaded benchmark
find_package(Threads REQUIRED)
target_link_libraries(benchmark_mthread PRIVATE Threads::Threads)

# Set include directories
target_include_directories(benchmark_simple PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(benchmark_mthread PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Set compiler-specific flags
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(benchmark_simple PRIVATE -stdlib=libc++)
    target_compile_options(benchmark_mthread PRIVATE -stdlib=libc++)
    target_link_options(benchmark_simple PRIVATE -stdlib=libc++)
    target_link_options(benchmark_mthread PRIVATE -stdlib=libc++)
endif()
