#include <benchmark/benchmark.h>
#include <ctime>
#include <functional>
#include "async_dictionary.hpp"
#include "dictionary.hpp"
#include "naive_async_dictionary.hpp"
#include "naive_dictionary.hpp"
#include "tools.hpp"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define PASTER(x, y) x##y
#define EVALUATOR(x, y) PASTER(x, y)

#define DIC EVALUATOR(VERSION, dictionary)
#define ASYNC_DIC EVALUATOR(VERSION, async_dictionary)

constexpr int NQUERIES = 1000;

class BMScenario : public ::benchmark::Fixture {
public:
	void SetUp(benchmark::State&)
	{
		if (!m_scenario) {
			auto wl = load_word_list();
			m_scenario = std::make_unique<Scenario>(wl, NQUERIES);
		}
	}

protected:
	static std::unique_ptr<Scenario> m_scenario;
};

std::unique_ptr<Scenario> BMScenario::m_scenario;

BENCHMARK_DEFINE_F(BMScenario, NoAsync)(benchmark::State& st)
{
	DIC dic;
	m_scenario->prepare(dic);

	for (auto _ : st)
		m_scenario->execute(dic);

	// std::cout << "Counter = " << dic.counter << std::endl;

	st.SetItemsProcessed(st.iterations() * NQUERIES);
}

BENCHMARK_DEFINE_F(BMScenario, Async)(benchmark::State& st)
{
	ASYNC_DIC dic;
	m_scenario->prepare(dic);

	for (auto _ : st)
		m_scenario->execute(dic);

	st.SetItemsProcessed(st.iterations() * NQUERIES);
}

BENCHMARK_DEFINE_F(BMScenario, NoAsync_MT)(benchmark::State& st)
{
	using namespace std::placeholders;

	std::srand(std::time(nullptr));

	std::vector<std::string> data = load_word_list();

	DIC dic(data.begin(), data.end());

	int nb_threads = 4; // nb thread per tasks (search, insert, erase)

	for (auto _ : st) {
		std::vector<std::thread> threads;
		for (auto n = 0; n < nb_threads; n++)
			threads.emplace_back([&dic, &data, n]() {
				// std::for_each(data.begin() + 0 * n,
				// data.begin() + 1 * n,
				// std::bind(&IDictionary::search, &dic, _1));
				for (size_t i = 0; i < NQUERIES * 70 / 100;
				     i++) {
					size_t index =
					    std::rand() % data.size();
					dic.search(data[index]);
				}
			});
		for (auto n = 0; n < nb_threads; n++)
			threads.emplace_back([&dic, &data, n]() {
				// std::for_each(data.begin() + 0 * n,
				// data.begin() + 1 * n,
				// std::bind(&IDictionary::search, &dic, _1));
				for (size_t i = 0; i < NQUERIES * 15 / 100;
				     i++) {
					size_t index =
					    std::rand() % data.size();
					dic.insert(data[index]);
				}
			});
		for (auto n = 0; n < nb_threads; n++)
			threads.emplace_back([&dic, &data, n]() {
				// std::for_each(data.begin() + 0 * n,
				// data.begin() + 1 * n,
				// std::bind(&IDictionary::search, &dic, _1));
				for (size_t i = 0; i < NQUERIES * 15 / 100;
				     i++) {
					size_t index =
					    std::rand() % data.size();
					dic.erase(data[index]);
				}
			});

		for (std::thread& t : threads)
			t.join();
	}
	// std::cout << "Counter = " << dic.counter << std::endl;

	st.SetItemsProcessed(st.iterations() * NQUERIES * nb_threads);
}

// BENCHMARK_REGISTER_F(BMScenario, NoAsync)->Unit(benchmark::kMillisecond);
// BENCHMARK_REGISTER_F(BMScenario, Async)->Unit(benchmark::kMillisecond);
BENCHMARK_REGISTER_F(BMScenario, NoAsync_MT)
    ->Unit(benchmark::kMillisecond)
    ->UseRealTime();

BENCHMARK_MAIN();
