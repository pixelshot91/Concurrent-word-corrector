#include "dictionary.hpp"
#include "tools.hpp"

dictionary::dictionary(const std::initializer_list<std::string>& init)
	: counter(0), reader()
{
	trie = std::make_shared<Node>("-");
	for (const std::string& s : init)
		trie->insert(s.data());
}


void dictionary::init(const std::vector<std::string>& word_list)
{
	//std::lock_guard l(m)s

	trie.reset(new Node("-"));
	for (const std::string& s : word_list)
		trie->insert(s.data());
}

result_t dictionary::search(const std::string& query) const
{
	int first_char = query[0] - 'a';
	{
		std::lock_guard l(m[first_char]);
		if (exist(query))
			return {query, 0};
	}
	//counter++;

	// Direct search
	/*if (exist(query)) {
		{
			std::lock_guard l(m[first_char]);
			reader[first_char]--;
		}
		cv[first_char].notify_all();
		return {query, 0};
	}*/

	// Levenhstein
	lv_array_t lv_array;
  
	std::string best;
  int distance = std::numeric_limits<int>::max();
	
	int width = query.size() + 1;
	lv_ctx lv_ctx = {"-" + query, lv_array, width, best, distance};
	//trie->lv(lv_ctx);

	{
		lv_array_t& array = lv_ctx.array;
		array.push_back({});
		auto& line = array.back();
		line.resize(lv_ctx.width);
		int l = array.size() - 1;
		line[0] = l;

		for (auto i = 1; i < lv_ctx.width; i++)
			array[0][i] = i;


		for (int i = 0 ; i < 26; i++) {
			const std::shared_ptr<Node>& c = trie->getChildren()[i];
			if (c.get())
			{
				{
					std::lock_guard l(m[i]);
					reader[i]++;
				}
				c->lv(lv_ctx);
				{
					std::lock_guard l(m[i]);
					reader[i]--;
				}
				cv[i].notify_all();
			}
		}

	}
  return {lv_ctx.best, lv_ctx.distance};
}


void dictionary::insert(const std::string& w)
{
	int first_char = w[0] - 'a';
	//std::cout << "Wait for insert " << w << std::endl;
  std::unique_lock l(m[first_char]);
	cv[first_char].wait(l, [this, first_char] { return this->reader[first_char] == 0; });
	//counter++;
	trie->insert(w.data());
	//std::cout << "Inserted : " << w << std::endl;
}

void dictionary::erase(const std::string& w)
{
	int first_char = w[0] - 'a';
	//std::cout << "Wait for erase " << w << std::endl;
  std::unique_lock l(m[first_char]);
	cv[first_char].wait(l, [this, first_char] { return this->reader[first_char] == 0; });
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
