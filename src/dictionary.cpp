#include "dictionary.hpp"
#include "tools.hpp"

dictionary::dictionary(const std::initializer_list<std::string>& init)
	: counter(0), reader(0)
{
	trie = std::make_shared<Node>("-");
	for (const std::string& s : init)
		trie->insert(s.data());
}


void dictionary::init(const std::vector<std::string>& word_list)
{
	std::lock_guard l(m);

	trie.reset(new Node("-"));
	for (const std::string& s : word_list)
		trie->insert(s.data());
}

result_t dictionary::search(const std::string& query) const
{
	{
		std::lock_guard l(m);
		reader++;
	}
	//counter++;

	// Direct search
	if (exist(query)) {
		{
			std::lock_guard l(m);
			reader--;
		}
		cv.notify_all();
		return {query, 0};
	}

	// Levenhstein
	lv_array_t lv_array;
  
	std::string best;
  int distance = std::numeric_limits<int>::max();
	
	int width = query.size() + 1;
	lv_ctx ctx = {"-" + query, lv_array, width, best, distance};
	trie->lv(ctx);
 
	//std::cout << "serach: " << ctx.best << std::endl;
	{
		std::lock_guard l(m);
		reader--;
	}
	cv.notify_all();

  return {ctx.best, ctx.distance};
}


void dictionary::insert(const std::string& w)
{
	//std::cout << "Wait for insert " << w << std::endl;
  std::unique_lock l(m);
	cv.wait(l, [this] { return this->reader == 0; });
	//counter++;
	trie->insert(w.data());
	//std::cout << "Inserted : " << w << std::endl;
}

void dictionary::erase(const std::string& w)
{
	//std::cout << "Wait for erase " << w << std::endl;
  std::unique_lock l(m);
	cv.wait(l, [this] { return this->reader == 0; });
	//counter++;
  trie->erase(w.data());
	//std::cout << "Erased : " << w << std::endl;
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
