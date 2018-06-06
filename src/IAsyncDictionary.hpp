#pragma once
#include <future>
#include "IDictionary.hpp"

class IAsyncDictionary : public IDictionaryBase {
public:
	IAsyncDictionary() = default;

	// clang-format off
	virtual std::future<result_t>	search(const std::string& w) const = 0;
	virtual std::future<void>	insert(const std::string& w) = 0;
	virtual std::future<void>	erase(const std::string& w) = 0;
	// clang-format on
};
