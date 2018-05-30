#pragma once
#include "IDictionary.hpp"
#include "node.hpp"
#include <set>
#include <mutex>
#include <vector>
#include <iostream>
#include <condition_variable>
#include <atomic>

class dictionary : public IDictionary
{
public:
  dictionary() = default;
  dictionary(const std::initializer_list<std::string>& init);

  template <class Iterator>
  dictionary(Iterator begin, Iterator end);

  void init(const std::vector<std::string>& word_list) final;

  result_t      search(const std::string& w) const final;
  void          insert(const std::string& w) final;
  void          erase(const std::string& w) final;

	mutable size_t counter;
private:
	bool exist(const std::string& w) const;

  std::shared_ptr<Node> trie;
	mutable std::mutex m;
	mutable std::atomic<int> reader;
	mutable std::condition_variable cv;
};

template <class Iterator>
dictionary::dictionary(Iterator begin, Iterator end)
	: counter(0), reader(0)
{
	trie = std::make_shared<Node>("");
	for (auto it = begin; it != end; it++) {
		trie->insert(it->data());
	}
}
