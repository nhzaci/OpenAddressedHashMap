#include <benchmark/benchmark.h>
#include <cstdlib>
#include <iostream>

#include <unordered_map>

#include "../src/OpenAddressedHashMap.hxx"

#define RANDOM_SEED 2

template <typename Map>
class HashMapBenchmarkFixture : public benchmark::Fixture {
public:
  Map m;

  void SetUp(const ::benchmark::State &state) {
    std::srand(RANDOM_SEED);
    for (size_t i = 0; i < state.range(0); i++) {
      m[std::rand()] = std::rand();
    }
  }

  void TearDown(const ::benchmark::State &state) {}
};

#define FIND_ITEM_FROM_MAP                                                     \
  (benchmark::State & st) {                                                    \
    for (auto _ : st) {                                                        \
      st.PauseTiming();                                                        \
      auto key = std::rand();                                                  \
      st.ResumeTiming();                                                       \
      auto itr = m.find(key);                                                  \
      benchmark::DoNotOptimize(itr);                                           \
      benchmark::ClobberMemory();                                              \
    }                                                                          \
  }

BENCHMARK_TEMPLATE_DEFINE_F(HashMapBenchmarkFixture, stdUnorderedMapFindTest,
                            std::unordered_map<int, int>)
FIND_ITEM_FROM_MAP

BENCHMARK_TEMPLATE_DEFINE_F(HashMapBenchmarkFixture, openAddressedMapFindTest,
                            nhzaci::open_addressed_hash_map<int, int>)
FIND_ITEM_FROM_MAP

#define WITH_RANGES Range(8, 8 << 22)

BENCHMARK_REGISTER_F(HashMapBenchmarkFixture, stdUnorderedMapFindTest)
    ->WITH_RANGES;
BENCHMARK_REGISTER_F(HashMapBenchmarkFixture, openAddressedMapFindTest)
    ->WITH_RANGES;

BENCHMARK_MAIN();