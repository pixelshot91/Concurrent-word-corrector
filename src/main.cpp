#include <cassert>

#include "dictionary.cpp"
#include "async_dictionary.cpp"
#include "naive_dictionary.cpp"
#include "naive_async_dictionary.cpp"

std::string to_string(result_t r) {
	return "(" + std::get<std::string>(r) + ", " + std::to_string(std::get<int>(r)) + ")";
}

int main()
{
	/*dictionary dic = {"irl"};
	auto res = dic.search("lr");
	std::cout << "Best match : '" << std::get<std::string>(res) << "' (" << std::to_string(std::get<int>(res)) << ")" << std::endl;*/

	/*dictionary dic = {
		"table", "tablier", "tableauter"
	};
	auto res = dic.search("talbe");
	std::cout << "Best match : '" << std::get<std::string>(res) << "' (" << std::to_string(std::get<int>(res)) << ")" << std::endl;*/
	
	std::vector<std::string> word_list = load_word_list();
  //word_list.resize(100000);
  word_list.resize(10);

  Scenario scn(word_list, 200);

  dictionary dic;
  naive_async_dictionary async_dic;
  scn.prepare(dic);
  scn.prepare(async_dic);
	std::cout << scn << std::endl;
  auto r1 = scn.execute(async_dic);
  auto r2 = scn.execute(dic);

	bool ok = true;
	
	if (r1.size() != r2.size()) {
		std::cout << "DIFFERENT SIZES" << std::endl;
		ok = false;
	}
  
	for (size_t i = 0; i < std::min(r1.size(), r2.size()); i++) {
		if (std::get<int>(r1[i]) != std::get<int>(r2[i])) {
			std::cout << "i=" << i << " " << to_string(r1[i]) << "\t" << to_string(r2[i]) << std::endl;
			ok = false;
		}
	}

	return !ok;
}
