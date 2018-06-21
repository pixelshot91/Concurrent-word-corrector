#pragma once

#include "IAsyncDictionary.hpp"
#include "dictionary.hpp"

/// The naive implementation is blocking to ensure Sequential Consistency
class async_dictionary : public IAsyncDictionary {
public:
	async_dictionary();

	template <class Iterator>
	async_dictionary(Iterator begin, Iterator end);
	async_dictionary(const std::initializer_list<std::string>& init);
	void init(const std::vector<std::string>& word_list) final;

	// clang-format off
	std::future<result_t>	search(const std::string& w) const final;
	std::future<void>	insert(const std::string& w) final;
	std::future<void>	erase(const std::string& w) final;
	// clang-format on
private:
	mutable std::mutex m;
	mutable std::atomic<int> reader;
	mutable std::atomic<int> inserter;
	mutable std::atomic<int> eraser;
	mutable std::condition_variable cv;
	dictionary m_dic;
};

template <class Iterator>
async_dictionary::async_dictionary(Iterator begin, Iterator end)
	: reader(0)
	, inserter(0)
	, eraser(0)
	, m_dic(begin, end)
{}
