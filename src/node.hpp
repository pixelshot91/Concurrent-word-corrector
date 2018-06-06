#pragma once
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

using lv_array_t = std::vector<std::vector<char>>;

struct lv_ctx {
	std::string query;
	lv_array_t array;
	int width;
	std::string best;
	int distance;

	void print(const std::string& s) const
	{
		int w = 3;
		std::cout << "  ";
		for (const char& c : query)
			std::cout << std::setw(w) << c;
		std::cout << std::endl;

		for (size_t l = 0; l < array.size(); l++) {
			if (l == 0)
				std::cout << "- ";
			else
				std::cout << s[l - 1] << " ";
			auto& line = array[l];
			for (size_t c = 0; c < line.size(); c++) {
				std::cout << std::setw(w) << line[c];
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
};

class Node {
public:
	Node(const std::string& str);
	~Node()
	{
		// std::cout << "Destructor " << s << std::endl;
	}
	// result_t search(const std::string& w);
	void insert(const char* w);

	// Return true if node can be deleted because it's not EOW and has no
	// children
	bool erase(const char* w);
	bool has_children() const;

	void lv(lv_ctx& lv_ctx) const;

	inline const std::shared_ptr<Node>& getChild(size_t i) const
	{
		return child[i];
	}
	inline const std::shared_ptr<Node>* getChildren() const
	{
		return child;
	}
	bool isEOW() const
	{
		return eow;
	}

private:
	std::shared_ptr<Node> child[26];
	bool eow;
	std::string s;
};
