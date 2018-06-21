#include <algorithm>
#include <iostream>

#include "node.hpp"

Node::Node(const std::string& str)
	: eow(false)
	, s(str)
{}

void Node::lv(lv_ctx& lv_ctx, const int l) const
{
	lv_array_t& array = lv_ctx.array;
	array.push_back(l);

	int width = lv_ctx.width;

	for (auto c = 1; c < lv_ctx.width; c++) {
		char cost = (s[l] != lv_ctx.query[c]);
		char min = std::min({array[width * (l - 1) + c] + 1,
				     array[width * l + (c - 1)] + 1,
				     array[width * (l - 1) + (c - 1)] + cost});

		if (l > 1 && c > 1 && s[l] == lv_ctx.query[c - 1] &&
		    s[l - 1] == lv_ctx.query[c]) {
			// std::cout << "SWAP possible" << std::endl;
			min = std::min(
			    min,
			    (char)(array[width * (l - 2) + (c - 2)] + cost));
		}

		array.push_back(min);
	}

	// lv_ctx.print(s);

	if (eow) {
		int dist = array.back();
		if (dist < lv_ctx.distance) {
			// std::cout << "Found NEW best ! " << s << " ("
			// << dist << ")" << std::endl;
			lv_ctx.best = s;
			lv_ctx.distance = dist;
		}
	}

	int node_size = s.size();
	int query_size = lv_ctx.query.size();
	int pt_to_loose = array.back() - lv_ctx.distance + 1;
	int uninitialized_distance = std::numeric_limits<int>::max();

	if (node_size > query_size &&
	    node_size + 1 - query_size >= lv_ctx.distance) {
	}
	else if (lv_ctx.distance != uninitialized_distance &&
		 node_size + pt_to_loose > query_size + lv_ctx.distance) {
		/*std::cout << "pt_to_loose = " << pt_to_loose << std::endl;
		std::cout << "Pruning" << std::endl;*/
	}
	else {
		for (auto& c : child) {
			if (c.get())
				c->lv(lv_ctx, l + 1);
		}
	}

	for (int i = 0; i < width; i++)
		array.pop_back();
	// array.resize(array.size() - query_size);
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
