#include "dictionary.hpp"
#include "tools.hpp"

dictionary::dictionary()
	: reader{0}
{
	trie = std::make_shared<Node>("-");
}
dictionary::dictionary(const std::initializer_list<std::string>& init)
	: reader{0}
{

	trie = std::make_shared<Node>("-");
	for (const std::string& s : init)
		trie->insert(s.data());
}

void dictionary::init(const std::vector<std::string>& word_list)
{
	// std::lock_guard l(m)s

	trie.reset(new Node("-"));
	for (const std::string& s : word_list)
		trie->insert(s.data());
}

result_t dictionary::search(const std::string& query) const
{
	{
		std::lock_guard l_root(m_root);
		if (query.size() == 0) {
			if (trie->isEOW())
				return {"", 0};
		}
		else {
			int first_char = query[0] - 'a';
			std::lock_guard l(m[first_char]);
			if (exist(query))
				return {query, 0};
		}
	}

	// Levenhstein
	int width = query.size() + 1;
	lv_array_t lv_array;
	lv_array.reserve(width * width * 2);

	std::string best;
	int uninitialized_distance = std::numeric_limits<int>::max();

	int distance = uninitialized_distance;

	lv_ctx lv_ctx = {"-" + query, lv_array, width, best, distance};

	{
		lv_array_t& array = lv_ctx.array;

		for (auto i = 0; i < lv_ctx.width; i++)
			array.push_back(i);

		for (int i = 0; i < 26; i++) {
			const std::shared_ptr<Node>& c = trie->getChildren()[i];
			if (c.get()) {
				{
					std::lock_guard l(m[i]);
					reader[i]++;
				}
				c->lv(lv_ctx, 1);
				{
					std::lock_guard l(m[i]);
					reader[i]--;
				}
				cv[i].notify_all();
			}
		}
	}
	if (lv_ctx.distance == uninitialized_distance)
		return {"", uninitialized_distance};
	return {lv_ctx.best.substr(1), lv_ctx.distance};
}

void dictionary::insert(const std::string& w)
{
	if (w.size() == 0) {
		std::lock_guard l_root(m_root);
		trie->insert(w.data());
		return;
	}

	auto first_char = w[0] - 'a';
	std::unique_lock l(m[first_char]);
	cv[first_char].wait(
	    l, [this, first_char] { return this->reader[first_char] == 0; });

	auto& child_ptr = trie->getChild(first_char);
	if (child_ptr.get() == nullptr)
		child_ptr = std::make_shared<Node>(std::string("-") + w[0]);
	child_ptr->insert(w.data() + 1);

	l.unlock();
	cv[first_char].notify_all();
}

void dictionary::erase(const std::string& w)
{
	if (w.size() == 0) {
		std::lock_guard l_root(m_root);
		trie->erase(w.data());
		return;
	}
	int first_char = w[0] - 'a';
	std::unique_lock l(m[first_char]);
	cv[first_char].wait(
	    l, [this, first_char] { return this->reader[first_char] == 0; });

	trie->erase(w.data());

	l.unlock();
	cv[first_char].notify_all();
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
