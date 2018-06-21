#include "async_dictionary.hpp"

async_dictionary::async_dictionary()
	: reader(0)
	, inserter(0)
	, eraser(0)
{}

async_dictionary::async_dictionary(
    const std::initializer_list<std::string>& init)
	: reader(0)
	, inserter(0)
	, eraser(0)
	, m_dic(init)
{}

void async_dictionary::init(const std::vector<std::string>& word_list)
{
	m_dic.init(word_list);
}

std::future<result_t> async_dictionary::search(const std::string& query) const
{
	{
		std::unique_lock l(m);
		cv.wait(l, [this] {
			if (inserter == 0 && eraser == 0) {
				reader++;
				return true;
			}
			return false;
		});
	}

	return std::async(std::launch::async, [this, query] {
		auto res = m_dic.search(query);
		{
			std::lock_guard g(m);
			reader--;
		}
		this->cv.notify_all();
		return res;
	});
}

std::future<void> async_dictionary::insert(const std::string& w)
{
	{
		std::unique_lock l(m);
		cv.wait(l, [this] {
			if (!reader && !eraser) {
				inserter++;
				return true;
			}
			return false;
		});
	}

	return std::async(std::launch::async, [this, w] {
		m_dic.insert(w);
		{
			std::lock_guard g(m);
			inserter--;
		}
		this->cv.notify_all();
	});
}

std::future<void> async_dictionary::erase(const std::string& w)
{
	{
		std::unique_lock l(m);
		cv.wait(l, [this] {
			if (!reader && !inserter) {
				eraser++;
				return true;
			}
			return false;
		});
	}

	return std::async(std::launch::async, [this, w] {
		m_dic.erase(w);
		{
			std::lock_guard g(m);
			eraser--;
		}
		this->cv.notify_all();
	});
}
