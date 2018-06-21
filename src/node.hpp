#pragma once
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

using lv_array_t = std::vector<char>;

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

		for (size_t l = 0; l < array.size() / width; l++) {
			std::cout << s[l] << " ";
			// auto& line = array[l];
			for (int c = 0; c < width; c++) {
				std::cout
				    << std::setw(w)
				    << std::to_string(array[l * width + c]);
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

	void lv(lv_ctx& lv_ctx, int line_no) const;

	inline std::shared_ptr<Node>& getChild(size_t i)
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
