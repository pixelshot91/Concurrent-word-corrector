cmake_minimum_required (VERSION 3.6)
project (ProgParallel)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")

find_package(Threads REQUIRED)
find_package(benchmark REQUIRED)
find_package(GTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-W -Wall -pthread")
set(CMAKE_CXX_FLAGS_DEBUG "-fsanitize=thread -O3 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

include_directories(lib/include)

add_library(naive_dictionary
  src/IDictionary.hpp
  src/IAsyncDictionary.hpp
  src/naive_dictionary.hpp
  src/naive_async_dictionary.hpp
  src/tools.hpp

  src/naive_dictionary.cpp
  src/naive_async_dictionary.cpp
  src/tools.cpp
	)

add_library(dictionary
  src/IDictionary.hpp
  src/IAsyncDictionary.hpp
  src/dictionary.hpp
  src/async_dictionary.hpp
  src/tools.hpp

  src/dictionary.cpp
  src/async_dictionary.cpp
  src/tools.cpp

	src/node.cpp
  )

set_source_files_properties(src/tools.cpp PROPERTIES COMPILE_DEFINITIONS WORD_LIST_FILE="${CMAKE_SOURCE_DIR}/words_alpha.txt")
add_executable(naive_tests src/naive_tests.cpp)
target_link_libraries(naive_tests naive_dictionary GTest::GTest GTest::Main Threads::Threads)

add_executable(tests src/tests.cpp)
target_link_libraries(tests dictionary GTest::GTest GTest::Main Threads::Threads)

add_executable(naive_bench src/bench.cpp)
add_executable(bench src/bench.cpp)
set_target_properties(naive_bench PROPERTIES COMPILE_FLAGS "-DVERSION=naive_")
set_target_properties(bench PROPERTIES COMPILE_FLAGS "-DVERSION=")
target_link_libraries(naive_bench naive_dictionary benchmark::benchmark)
target_link_libraries(bench dictionary benchmark::benchmark)

add_executable(main src/main.cpp)
target_link_libraries(main dictionary)
