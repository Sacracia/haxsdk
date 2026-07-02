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

class MapBenchmark : public ::testing::Test
{
protected:
    void RunSuite(int N)
    {
        std::vector<int> keys(N);
        std::iota(keys.begin(), keys.end(), 0);
        std::mt19937 gen(42);
        std::shuffle(keys.begin(), keys.end(), gen);

        const int iterations = 100000 / (N > 0 ? N : 1);

        BenchmarkInsertCustom(keys, iterations, N);
        BenchmarkInsertStd(keys, iterations, N);

        Map<int, int> customMap;
        std::map<int, int> stdMap;

        customMap.Reserve(N);

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
            Map<int, int> m;
            m.Reserve(N);
            for (int k : keys)
            {
                m.Insert(k, k);
            }
            ForceUse(m.Size());
        }
        auto end = std::chrono::high_resolution_clock::now();
        Report("Map Insert", N, start, end, iters);
    }

    void BenchmarkInsertStd(const std::vector<int>& keys, int iters, int N)
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iters; ++i)
        {
            std::map<int, int> m;
            for (int k : keys)
            {
                m[k] = k;
            }
            ForceUse(m.size());
        }
        auto end = std::chrono::high_resolution_clock::now();
        Report("std::map Insert", N, start, end, iters);
    }

    void BenchmarkGetCustom(Map<int, int>& m, const std::vector<int>& keys, int iters)
    {
        int sum = 0;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iters; ++i)
        {
            for (int k : keys)
            {
                int* v = m.Get(k);
                if (v) sum += *v;
            }
            if (sum == 0x7FFFFFFF) printf(" ");
        }
        auto end = std::chrono::high_resolution_clock::now();
        ForceUse(sum);
        Report("Map Get", (int)keys.size(), start, end, iters);
    }

    void BenchmarkGetStd(std::map<int, int>& m, const std::vector<int>& keys, int iters)
    {
        int sum = 0;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iters; ++i)
        {
            for (int k : keys)
            {
                auto it = m.find(k);
                if (it != m.end()) sum += it->second;
            }
            if (sum == 0x7FFFFFFF) printf(" ");
        }
        auto end = std::chrono::high_resolution_clock::now();
        ForceUse(sum);
        Report("std::map Find", (int)keys.size(), start, end, iters);
    }

    void BenchmarkIterateCustom(Map<int, int>& m, int iters)
    {
        int sum = 0;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iters; ++i)
        {
            for (auto const& entry : m)
            {
                sum += entry.value;
            }
            if (sum == 0x7FFFFFFF) printf(" ");
        }
        auto end = std::chrono::high_resolution_clock::now();
        ForceUse(sum);
        Report("Map Iter", (int)m.Size(), start, end, iters);
    }

    void BenchmarkIterateStd(std::map<int, int>& m, int iters)
    {
        int sum = 0;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iters; ++i)
        {
            for (auto const& [key, value] : m)
            {
                sum += value;
            }
            if (sum == 0x7FFFFFFF) printf(" ");
        }
        auto end = std::chrono::high_resolution_clock::now();
        ForceUse(sum);
        Report("std::map Iter", (int)m.size(), start, end, iters);
    }

    void Report(const char* label, int N, std::chrono::steady_clock::time_point s, std::chrono::steady_clock::time_point e, int iters)
    {
        auto total_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(e - s).count();
        double avg = (double)total_ns / iters;
        printf("[ %-16s ] N = %-4d | Avg: %10.2f ns\n", label, N, avg);
    }
};

TEST_F(MapBenchmark, RunAll)
{
    int sizes[] = { 10, 50, 100 };
    for (int n : sizes)
    {
        RunSuite(n);
        printf("------------------------------------------------------------\n");
    }
}