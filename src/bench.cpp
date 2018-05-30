#include <functional>
#include "tools.hpp"
#include "naive_dictionary.hpp"
#include "naive_async_dictionary.hpp"
#include "dictionary.hpp"
#include "async_dictionary.hpp"

#include <benchmark/benchmark.h>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define PASTER(x,y) x ## y
#define EVALUATOR(x,y)  PASTER(x,y)
#define DIC EVALUATOR(VERSION, dictionary)

constexpr int NQUERIES = 10;

class BMScenario : public ::benchmark::Fixture
{
public:
  void SetUp(benchmark::State&)
    {
      if (!m_scenario)
      {
        auto wl = load_word_list();
        m_scenario = std::make_unique<Scenario>(wl, NQUERIES);
      }
    }

protected:
  static std::unique_ptr<Scenario> m_scenario;
};

std::unique_ptr<Scenario> BMScenario::m_scenario;

BENCHMARK_DEFINE_F(BMScenario, Optimized_NoAsync)(benchmark::State& st)
{
	DIC dic;
  m_scenario->prepare(dic);

  for (auto _ : st)
    m_scenario->execute(dic);

  st.SetItemsProcessed(st.iterations() * NQUERIES);
}

BENCHMARK_DEFINE_F(BMScenario, Optimized_Async)(benchmark::State& st)
{
  DIC dic;
  m_scenario->prepare(dic);

  for (auto _ : st)
    m_scenario->execute(dic);

  st.SetItemsProcessed(st.iterations() * NQUERIES);
}

/*BENCHmARK_DEFINE_F()
{
	using namespace std::placeholders;

  std::vector<std::string> data = load_word_list();
  std::size_t n = 1000;
  // Cut in 3 parts: A B C
  // Initialize with A B
  // 2 threads Search A
  // 2 threads Remove B
  // 2 threads Insert C

  dictionary dic(data.begin(), data.begin() + 4 * n);

  std::thread t[6];

  t[0] = std::thread([&dic,&data,n]() { std::for_each(data.begin() + 0 * n, data.begin() + 1 * n, std::bind(&IDictionary::search, &dic, _1)); });
  t[1] = std::thread([&dic,&data,n]() { std::for_each(data.begin() + 1 * n, data.begin() + 2 * n, std::bind(&IDictionary::search, &dic, _1)); });

  t[2] = std::thread([&dic,&data, n]() { std::for_each(data.begin() + 2 * n, data.begin() + 3 * n, std::bind(&IDictionary::erase, &dic, _1)); });
  t[3] = std::thread([&dic,&data, n]() { std::for_each(data.begin() + 3 * n, data.begin() + 4 * n, std::bind(&IDictionary::erase, &dic, _1)); });

  t[4] = std::thread([&dic,&data, n]() { std::for_each(data.begin() + 4 * n, data.begin() + 5 * n, std::bind(&IDictionary::insert, &dic, _1)); });
  t[5] = std::thread([&dic,&data, n]() { std::for_each(data.begin() + 5 * n, data.begin() + 6 * n, std::bind(&IDictionary::insert, &dic, _1)); });

  for (int i = 0; i < 6; ++i)
    t[i].join();
	
  st.SetItemsProcessed(st.iterations() * NQUERIES);
}*/

BENCHMARK_REGISTER_F(BMScenario, Optimized_NoAsync)->Unit(benchmark::kMillisecond);
BENCHMARK_REGISTER_F(BMScenario, Optimized_Async)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
