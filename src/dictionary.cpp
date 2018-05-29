#include "dictionary.hpp"
#include "tools.hpp"

dictionary::dictionary(const std::initializer_list<std::string>& init)
{
	trie = std::make_shared<Node>("");
	for (const std::string& s : init)
		trie->insert(s.data());
}


void dictionary::init(const std::vector<std::string>& word_list)
{
	std::lock_guard l(m);

	trie.reset(new Node(""));
	for (const std::string& s : word_list)
		trie->insert(s.data());
}

result_t dictionary::search(const std::string& query) const
{
  std::lock_guard l(m);

	// Direct search
	if (exist(query)) {
		return {query, 0};
	}

	// Levenhstein
	lv_array_t lv_array;
  
	std::string best;
  int distance = std::numeric_limits<int>::max();
	
	int width = query.size() + 1;
	lv_ctx ctx = {"-" + query, lv_array, width, best, distance};
	trie->lv(ctx);
  
  return {ctx.best, ctx.distance};
}


void dictionary::insert(const std::string& w)
{
  std::lock_guard l(m);
	trie->insert(w.data());
}

void dictionary::erase(const std::string& w)
{
  std::lock_guard l(m);
  trie->erase(w.data());
}

bool dictionary::exist(const std::string& w) const
{
	auto cur_node(trie);
	for (const char& c : w) {
		if (!cur_node->getChild(c - 'a').get())
			return false;
		cur_node = cur_node->getChild(c - 'a');
	}
	return cur_node->isEOW();
}
