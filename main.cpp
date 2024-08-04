#include <iostream>
#include <string>
#include <chrono>
#include <random>
#include <unordered_map>
#include "ZZG_Hash.h"  // Assuming this is your custom hash implementation

#define LOOPS 1024*1024*3/4
#define SHIFT 5

std::string generate_random_string(std::mt19937_64& gen, int length) {
    static const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    std::string result(length, 0);
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    for (int i = 0; i < length; ++i) {
        result[i] = charset[dist(gen)];
    }

    return result;
}

int main() {
    ZZG::zHash<std::string, size_t> MyHash;
    std::unordered_map<std::string, size_t> stdHash;
    size_t Buckets, FilledBuckets, Elements, Collisions, MaxCollision;

    // Check time points to evaluate performance
    auto T0 = std::chrono::high_resolution_clock::now();

    // Create random strings
    std::mt19937_64 gen(std::random_device{}());
    std::vector<std::string> Keys(LOOPS);

    for (size_t i = 0; i < LOOPS; ++i) {
        Keys[i] = generate_random_string(gen, 16);
    }

    auto T1 = std::chrono::high_resolution_clock::now();

    // Test ZZG::zHash
    for (size_t i = 0; i < LOOPS; ++i) {
        MyHash.Insert(Keys[i], i);
    }

    size_t Value;
    for (size_t i = 0; i < LOOPS; ++i) {
        MyHash.Value(Keys[i], &Value);
    }

    auto T2 = std::chrono::high_resolution_clock::now();

    // Test std::unordered_map
    for (size_t i = 0; i < LOOPS; ++i) {
        stdHash[Keys[i]] = i;
    }

    for (size_t i = 0; i < LOOPS; ++i) {
        Value = stdHash[Keys[i]];
    }

    auto T3 = std::chrono::high_resolution_clock::now();

    // Calculate and print timing results
    auto gen_time = std::chrono::duration_cast<std::chrono::milliseconds>(T1 - T0).count();
    auto zHash_time = std::chrono::duration_cast<std::chrono::milliseconds>(T2 - T1).count();
    auto stdHash_time = std::chrono::duration_cast<std::chrono::milliseconds>(T3 - T2).count();

    std::cout << "zHash time: " << zHash_time 
              << "; stdHash time: " << stdHash_time 
              << "; Gen time: " << gen_time << "\n";

    // Check ZZG::zHash statistics
    MyHash.CheckHash(Buckets, FilledBuckets, Elements, Collisions, MaxCollision);
    std::cout << "Buckets=" << Buckets 
              << ", FilledBuckets=" << FilledBuckets 
              << ", Elements=" << Elements 
              << ", Collisions=" << Collisions 
              << ", MaxCollision=" << MaxCollision << "\n";

    return 0;
}