#include <algorithm>
#include <iostream>

#include "node.hpp"

Node::Node(const std::string& str)
	: eow(false)
	, s(str)
{
}

/*result_t Node::search(const std:string& w)
{
	if (w.empty())
		return {s, 0};
	auto index = w[0] - 'a';
	child[index]
}
*/
void Node::lv(lv_ctx& lv_ctx) const
{
	lv_array_t& array = lv_ctx.array;
	array.push_back({});
	auto& line = array.back();
	line.resize(lv_ctx.width);
	int l = array.size() - 1;
	line[0] = l;

	if (l == 0) {
		for (auto i = 1; i < lv_ctx.width; i++)
			array[0][i] = i;
	}
	else {
		auto& prec_line = array[l - 1];
		for (auto c = 1; c < lv_ctx.width; c++) {
			char cost = (s[l] != lv_ctx.query[c]);
			char min = std::min({prec_line[c] + 1, line[c - 1] + 1,
					     prec_line[c - 1] + cost});

			if (l > 1 && c > 1 && s[l] == lv_ctx.query[c - 1] &&
			    s[l - 1] == lv_ctx.query[c]) {
				// std::cout << "SWAP possible" << std::endl;
				min = std::min(
				    min, (char)(array[l - 2][c - 2] + cost));
			}

			line[c] = min;
		}
	}

	// lv_ctx.print(s);

	if (eow) {
		int dist = line.back();
		if (dist < lv_ctx.distance) {
			// std::cout << "Found NEW best ! " << s << " ("
			// << dist << ")" << std::endl;
			lv_ctx.best = s;
			lv_ctx.distance = dist;
		}
	}
	
	int node_size = s.size();
	int query_size = lv_ctx.query.size()-1;
	
	if (node_size > query_size &&
	    node_size - query_size > lv_ctx.distance) {
	}
	else {
		for (auto& c : child) {
			if (c.get())
				c->lv(lv_ctx);
		}
	}

	array.pop_back();
}

void Node::insert(const char* w)
{
	if (w[0] == 0)
		eow = true;
	else {
		auto index = w[0] - 'a';
		if (child[index].get() == nullptr)
			child[index] = std::make_shared<Node>(s + w[0]);
		child[index]->insert(w + 1);
	}
}

bool Node::erase(const char* w)
{
	if (w[0] == 0) {
		eow = false;
		return has_children();
	}

	int index = w[0] - 'a';
	if (!child[index].get())
		return true;

	bool child_has_children = child[index]->erase(w + 1);

	if (!child_has_children) {
		child[index] = nullptr;
		return eow || has_children();
	}

	return true;
}

bool Node::has_children() const
{
	for (int i = 0; i < 26; i++)
		if (child[i].get())
			return true;
	return false;
}
