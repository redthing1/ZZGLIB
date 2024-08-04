#include "ZZG_Hash.h" // Assuming this is your custom hash implementation
#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <unordered_map>
#include <vector>

const size_t NUM_OPERATIONS = 1000000;
const size_t KEY_RANGE = 1000000;

std::mutex mtx;
std::atomic<size_t> total_operations(0);

// Benchmark function for STL unordered_map with mutex
void benchmark_stl_map(std::unordered_map<size_t, size_t>& map, bool is_writer) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, KEY_RANGE - 1);

  for (size_t i = 0; i < NUM_OPERATIONS; ++i) {
    size_t key = dis(gen);
    if (is_writer) {
      std::lock_guard<std::mutex> lock(mtx);
      map[key] = i;
    } else {
      std::lock_guard<std::mutex> lock(mtx);
      volatile size_t value = map[key];
      (void) value; // Prevent optimization
    }
    total_operations.fetch_add(1, std::memory_order_relaxed);
  }
}

// Benchmark function for ZZG::zHash
void benchmark_zzg_hash(ZZG::zHash<size_t, size_t>& hash, bool is_writer) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, KEY_RANGE - 1);

  for (size_t i = 0; i < NUM_OPERATIONS; ++i) {
    size_t key = dis(gen);
    if (is_writer) {
      hash.Insert(key, i);
    } else {
      size_t value;
      hash.Value(key, &value);
    }
    total_operations.fetch_add(1, std::memory_order_relaxed);
  }
}

// Run benchmark with given number of threads
template <typename HashType, typename BenchmarkFunc>
double run_benchmark(size_t num_threads, HashType& hash, BenchmarkFunc benchmark_func) {
  std::vector<std::thread> threads;
  total_operations.store(0);

  auto start = std::chrono::high_resolution_clock::now();

  // Create threads (half writers, half readers)
  for (size_t i = 0; i < num_threads; ++i) {
    threads.emplace_back(benchmark_func, std::ref(hash), i < num_threads / 2);
  }

  // Join threads
  for (auto& thread : threads) {
    thread.join();
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diff = end - start;

  double ops_per_second = total_operations.load() / diff.count();
  return ops_per_second;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <num_threads>\n";
    return 1;
  }

  size_t num_threads = std::stoul(argv[1]);

  std::unordered_map<size_t, size_t> stl_map;
  ZZG::zHash<size_t, size_t> zzg_hash;

  std::cout << "Running benchmark with " << num_threads << " threads\n";

  // Benchmark STL unordered_map with mutex
  double stl_ops_per_second = run_benchmark(num_threads, stl_map, benchmark_stl_map);
  std::cout << "STL unordered_map: " << stl_ops_per_second << " ops/sec\n";

  // Benchmark ZZG::zHash
  double zzg_ops_per_second = run_benchmark(num_threads, zzg_hash, benchmark_zzg_hash);
  std::cout << "ZZG::zHash: " << zzg_ops_per_second << " ops/sec\n";

  std::cout << "Speedup: " << (zzg_ops_per_second / stl_ops_per_second) << "x\n";

  return 0;
}