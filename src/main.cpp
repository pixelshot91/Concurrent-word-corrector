#include "dictionary.cpp"

int main()
{
	dictionary dic = {
		"table", "tablier", "tableauter"
	};
	auto res = dic.search("talbe");
	std::cout << "Best match : '" << std::get<std::string>(res) << "' (" << std::to_string(std::get<int>(res)) << ")" << std::endl;
	return 0;
}
