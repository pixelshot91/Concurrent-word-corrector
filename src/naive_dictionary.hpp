#pragma once
#include <mutex>
#include <set>
#include <vector>
#include "IDictionary.hpp"

class naive_dictionary : public IDictionary {
public:
	naive_dictionary() = default;
	naive_dictionary(const std::initializer_list<std::string>& init);

	template <class Iterator>
	naive_dictionary(Iterator begin, Iterator end);

	void init(const std::vector<std::string>& word_list) final;

	// clang-format off
	result_t	search(const std::string& w) const final;
	void		insert(const std::string& w) final;
	void		erase(const std::string& w) final;
	// clang-format on
private:
	std::set<std::string> m_set;
	mutable std::mutex m;
};

template <class Iterator>
naive_dictionary::naive_dictionary(Iterator begin, Iterator end)
	: m_set(begin, end)
{}
