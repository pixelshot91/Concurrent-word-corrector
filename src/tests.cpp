#include <functional>
#include <gtest/gtest.h>
#include <thread>
#include "async_dictionary.hpp"
#include "dictionary.hpp"
#include "naive_dictionary.hpp"
#include "tools.hpp"

using namespace std::string_literals;

// TODO
// Adapt/Create new tests tests with your new structures
// naive_dictionary/async_naive_dictionary can be used as references

// A basic add/remove/search test
TEST(Dictionary, Basic)
{
	dictionary dic = {"massue", "lamasse", "massive"};

	ASSERT_EQ(dic.search("massive"), std::make_pair("massive"s, 0));
	ASSERT_EQ(dic.search("lessive"), std::make_pair("massive"s, 2));
	ASSERT_EQ(dic.search("limace"), std::make_pair("lamasse"s, 3));
	ASSERT_EQ(dic.search("masseur"), std::make_pair("massue"s, 2));

	dic.insert("masseur");
	ASSERT_EQ(dic.search("masseur"), std::make_pair("masseur"s, 0));

	dic.erase("masseur");
	ASSERT_EQ(dic.search("masseur"), std::make_pair("massue"s, 2));
}

// Test that executes some operations concurrently
// Does not test anything in itself but can be used to detected bugs
// with the thread sanitizer enabled
TEST(Dictionary, ConcurrentOperations)
{
	using namespace std::placeholders;

	std::vector<std::string> data = load_word_list();
	const std::size_t chk_sz = 1000;
	// Cut in 3 parts: A B C
	// Initialize with A B
	// 2 threads Search A
	// 2 threads Remove B
	// 2 threads Insert C

	dictionary dic(data.begin(), data.begin() + 4 * chk_sz);

	std::thread t[6];

	t[0] = std::thread([&dic, &data, chk_sz]() {
		std::for_each(data.begin() + 0 * chk_sz,
			      data.begin() + 1 * chk_sz,
			      std::bind(&IDictionary::search, &dic, _1));
	});
	t[1] = std::thread([&dic, &data, chk_sz]() {
		std::for_each(data.begin() + 1 * chk_sz,
			      data.begin() + 2 * chk_sz,
			      std::bind(&IDictionary::search, &dic, _1));
	});

	t[2] = std::thread([&dic, &data, chk_sz]() {
		std::for_each(data.begin() + 2 * chk_sz,
			      data.begin() + 3 * chk_sz,
			      std::bind(&IDictionary::erase, &dic, _1));
	});
	t[3] = std::thread([&dic, &data, chk_sz]() {
		std::for_each(data.begin() + 3 * chk_sz,
			      data.begin() + 4 * chk_sz,
			      std::bind(&IDictionary::erase, &dic, _1));
	});

	t[4] = std::thread([&dic, &data, chk_sz]() {
		std::for_each(data.begin() + 4 * chk_sz,
			      data.begin() + 5 * chk_sz,
			      std::bind(&IDictionary::insert, &dic, _1));
	});
	t[5] = std::thread([&dic, &data, chk_sz]() {
		std::for_each(data.begin() + 5 * chk_sz,
			      data.begin() + 6 * chk_sz,
			      std::bind(&IDictionary::insert, &dic, _1));
	});

	for (int i = 0; i < 6; ++i)
		t[i].join();
}

// A simple scenario
TEST(Dictionary, SimpleScenario)
{
	std::vector<std::string> word_list = load_word_list();
	word_list.resize(20);

	Scenario scn(word_list, 20);

	dictionary dic;
	scn.prepare(dic);
	scn.execute_verbose(dic);
}

// A long scenario, check that the async dictionary has the
// same output as the blocking one
TEST(Dictionary, LongScenario)
{
	std::vector<std::string> word_list = load_word_list();
	word_list.resize(10);

	Scenario scn(word_list, 1E5);

	naive_dictionary ref_dic;
	dictionary sync;
	async_dictionary async;

	scn.prepare(ref_dic);
	scn.prepare(sync);
	scn.prepare(async);

	auto r_ref = scn.execute(ref_dic);
	auto r_sync = scn.execute(sync);
	auto r_async = scn.execute(async);

	ASSERT_EQ(r_ref.size(), r_sync.size());
	ASSERT_EQ(r_ref.size(), r_async.size());

	for (size_t i = 0; i < r_ref.size(); i++) {
		// std::cout << "i= " << i << ": " << r1[i] << ", " << r2[i] <<
		// std::endl;
		ASSERT_EQ(std::get<int>(r_ref[i]), std::get<int>(r_sync[i]));
		ASSERT_EQ(std::get<int>(r_ref[i]), std::get<int>(r_async[i]));
	}
}

TEST(Dictionary, Async_Sequential_Consistency)
{
	int uninit_dist = std::numeric_limits<int>::max();
	async_dictionary dic;
	std::string w = "bonjour";
	for (int i = 0; i < 5; i++) {
		auto s1 = dic.search(w);
		dic.insert(w);
		auto s2 = dic.search(w);
		dic.erase(w);
		auto s3 = dic.search(w);

		ASSERT_EQ(s1.get(), result_t({"", uninit_dist}));
		ASSERT_EQ(s2.get(), result_t({w, 0}));
		ASSERT_EQ(s3.get(), result_t({"", uninit_dist}));
	}
}
