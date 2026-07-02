#include "../pch.h"

#include <map>
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>
#include <chrono>
#include <unordered_map>

using namespace Hax;

#pragma optimize("", off)
template <typename T>
void ForceUse(T const& value)
{
    volatile const T* dummy = &value;
}
#pragma optimize("", on)

class HashMapBenchmark : public ::testing::Test
{
protected:
    void RunSuite(int N)
    {
        std::vector<int> keys(N);
        std::iota(keys.begin(), keys.end(), 0);
        std::mt19937 gen(42);
        std::shuffle(keys.begin(), keys.end(), gen);

        const int iterations = 200000 / (N > 0 ? N : 1);

        BenchmarkInsertCustom(keys, iterations, N);
        BenchmarkInsertStd(keys, iterations, N);

        HashMap<int, int> customMap;
        std::unordered_map<int, int> stdMap;
        customMap.Reserve(N);
        stdMap.reserve(N);

        for (int k : keys)
        {
            customMap.Insert(k, k);
            stdMap[k] = k;
        }

        BenchmarkGetCustom(customMap, keys, iterations);
        BenchmarkGetStd(stdMap, keys, iterations);

        BenchmarkIterateCustom(customMap, iterations);
        BenchmarkIterateStd(stdMap, iterations);
    }

private:
    void BenchmarkInsertCustom(const std::vector<int>& keys, int iters, int N)
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iters; ++i)
        {
            HashMap<int, int> map;
            map.Reserve(N);
            for (int k : keys)
            {
                map.Insert(k, k);
            }
            ForceUse(map.Size());
        }
        auto end = std::chrono::high_resolution_clock::now();
        Report("Custom Insert", N, start, end, iters);
    }

    void BenchmarkInsertStd(const std::vector<int>& keys, int iters, int N)
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iters; ++i)
        {
            std::unordered_map<int, int> map;
            map.reserve(N);
            for (int k : keys)
            {
                map[k] = k;
            }
            ForceUse(map.size());
        }
        auto end = std::chrono::high_resolution_clock::now();
        Report("Std Insert", N, start, end, iters);
    }

    void BenchmarkGetCustom(HashMap<int, int>& map, const std::vector<int>& keys, int iters)
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iters; ++i)
        {
            int sum = 0;
            for (int k : keys)
            {
                int* v = map.Get(k);
                if (v) sum += *v;
                if (sum == 0x7FFFFFFF) printf(" ");
            }
            ForceUse(sum);
        }
        auto end = std::chrono::high_resolution_clock::now();
        Report("Custom Get", (int)keys.size(), start, end, iters);
    }

    void BenchmarkGetStd(std::unordered_map<int, int>& map, const std::vector<int>& keys, int iters)
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iters; ++i)
        {
            int sum = 0;
            for (int k : keys)
            {
                auto it = map.find(k);
                if (it != map.end()) sum += it->second;
                if (sum == 0x7FFFFFFF) printf(" ");
            }
            ForceUse(sum);
        }
        auto end = std::chrono::high_resolution_clock::now();
        Report("Std Find", (int)keys.size(), start, end, iters);
    }

    void BenchmarkIterateCustom(HashMap<int, int>& map, int iters)
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iters; ++i)
        {
            int sum = 0;
            for (auto it = map.begin(); it != map.end(); ++it)
            {
                sum += it->Value;
                if (sum == 0x7FFFFFFF) printf(" ");
            }
            ForceUse(sum);
        }
        auto end = std::chrono::high_resolution_clock::now();
        Report("Custom Iter", (int)map.Size(), start, end, iters);
    }

    void BenchmarkIterateStd(std::unordered_map<int, int>& map, int iters)
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iters; ++i)
        {
            int sum = 0;
            for (auto const& pair : map)
            {
                sum += pair.second;
                if (sum == 0x7FFFFFFF) printf(" ");
            }
            ForceUse(sum);
        }
        auto end = std::chrono::high_resolution_clock::now();
        Report("Std Iter", (int)map.size(), start, end, iters);
    }

    void Report(const char* label, int N, std::chrono::steady_clock::time_point s, std::chrono::steady_clock::time_point e, int iters)
    {
        auto total_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(e - s).count();
        double avg = (double)total_ns / iters;
        printf("[ %-15s ] N = %-4d | Avg: %10.2f ns\n", label, N, avg);
    }
};

TEST_F(HashMapBenchmark, RunAll)
{
    int sizes[] = {10, 50, 100};
    for (int n : sizes)
    {
        RunSuite(n);
        printf("----------------------------------------------------------\n");
    }
}
