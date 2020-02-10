// Copyright (c) 2020 Kurt Miller (kurtjm@gmx.com)
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <memory>

#include "symbol_map.h"
#include "rhyme_dictionary.h"

int main() {
  // the data files are expected in the following locations
  // relative to the working directory;
  auto const dictionary_file = "data/cmudict-0.7b";
  auto const symbol_file = "data/cmudict-0.7b.symbols";

  std::cout << "Rhyme finder test\nLoading dictionary..." << std::flush;

  auto dictionary = std::unique_ptr<Rhyme_dictionary>(new Rhyme_dictionary());
  if (!dictionary->load(dictionary_file, symbol_file)) {
    std::cout << "-failed.\n";
    return 1;
  }

  std::cout << "-done!  Word count: " << dictionary->get_word_count() << "\n\n";

  while(1) {
    auto word = std::string{};
    std::cout << "Enter a word to rhyme against (enter to quit): ";
    getline(std::cin, word);

    if (word.empty()) {
      break;
    }

    if (!dictionary->has_word(word)) {
      std::cout << "\"" << word << "\" is not in the dictionary.\n\n";
      continue;
    }

    auto exact = std::string{};
    std::cout << "Search for exact rhymes only? [y/n, default = y]: ";
    getline(std::cin, exact);

    auto match_full = false;
    if (!exact.empty() && tolower(exact.at(0)) == 'n') {
      match_full = true;
    }

    auto results = dictionary->find_rhymes(word, match_full);
    std::cout << results.size() << " rhymes found.\n";
    for (auto const& result : results) {
      std::cout << result << ", ";
    }
    std::cout << "\n\n";
  }
  return 0;
}
