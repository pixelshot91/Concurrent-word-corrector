#pragma once
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <set>
#include <vector>
#include "IDictionary.hpp"
#include "node.hpp"

class dictionary : public IDictionary {
public:
  dictionary();
  dictionary(const std::initializer_list<std::string>& init);

	template <class Iterator>
	dictionary(Iterator begin, Iterator end);

	void init(const std::vector<std::string>& word_list) final;

	result_t search(const std::string& w) const final;
	void insert(const std::string& w) final;
	void erase(const std::string& w) final;

private:
	bool exist(const std::string& w) const;

  std::shared_ptr<Node> trie;
	mutable std::mutex m_root;
	mutable std::mutex m[26];
	mutable unsigned char reader[26];
	mutable std::condition_variable cv[26];
};

template <class Iterator>
dictionary::dictionary(Iterator begin, Iterator end)
	: reader{0}
{

	trie = std::make_shared<Node>("-");
	for (auto it = begin; it != end; it++) {
		trie->insert(it->data());
	}
}
