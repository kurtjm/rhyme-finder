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

#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <assert.h>

#include "symbol_map.h"
#include "rhyme_dictionary.h"

Rhyme_dictionary::Rhyme_dictionary() : m_symbol_map(new Symbol_map) {
}

Rhyme_dictionary::~Rhyme_dictionary() {
}

std::string strip_variant(const std::string& word) {
  auto pos = word.find_first_of('(');
  if (pos != std::string::npos) {
    return word.substr(0, pos);
  }
  return word;
}

bool Rhyme_dictionary::load(const std::string& dictionary_file, const std::string& symbol_file) {
  clear();
  if (!m_symbol_map->load_symbols(symbol_file)) {
    return false;
  }

  auto input = std::ifstream(dictionary_file);
  if (!input) {
    return false;
  }

  auto line = std::string{};
  while (std::getline(input, line)) {
    auto pos = line.find(' ');
    if ((pos != std::string::npos) && (pos > 0)) {
      auto word = line.substr(0, pos);
      if (!isalpha(word.at(0))) {
        continue;
      }
      // find the split between the word and its symbols;
      pos = line.find_first_not_of(' ', pos);
      if (pos == std::string::npos) {
        continue;
      }
      // process the word and symbols;
      if (!process_word(strip_variant(word), line.substr(pos))) {
        return false;
      }
    }
  }
  return true;
}

int Rhyme_dictionary::get_word_count() const {
  return m_word_count;
}

bool Rhyme_dictionary::has_word(const std::string& word) const {
  auto word_upper = std::string{word};
  std::transform(std::begin(word_upper), std::end(word_upper), std::begin(word_upper), toupper);
  return (m_word_symbols.find(word_upper) != std::end(m_word_symbols));
}

std::vector<std::string> Rhyme_dictionary::find_rhymes(const std::string& word, bool match_full) const {
  auto word_upper = std::string{word};
  std::transform(std::begin(word_upper), std::end(word_upper), std::begin(word_upper), toupper);

  auto results = Word_results{};
  auto iter = m_word_symbols.find(word_upper);
  if (iter != std::end(m_word_symbols)) {
    auto const& variants = iter->second;
    for (auto const& symbolized_word : variants) {
      auto symbol_count = static_cast<int>(symbolized_word.symbols.size());
      auto end = 0;
      for (auto i=0; i<symbol_count; i++) {
        // find the first vowel as an ending point;
        if (!m_symbol_map->is_consonant(symbolized_word.symbols[i])) {
          end = i;
          break;
        }
      }
      get_nodes(m_nodes, symbol_count - 1, end, symbolized_word.symbols, results, match_full);
    }
  }
  return std::vector<std::string>{std::begin(results), std::end(results)};
}

bool Rhyme_dictionary::process_word(const std::string& word, const std::string& line) {
  auto symbolized_word = Rhyme_dictionary::Symbolized_word{};

  // this steps through each space-separated symbol, converting each to its
  // corresponding id and adding it to the symbolized version of the word;
  auto len = line.length();
  auto start = 0;

  auto next_end = [&line, &len, &start]() {
    auto pos = line.find_first_of(' ', start);
    return (pos != std::string::npos) ? pos : len;
  };

  auto end = next_end();
  while (start < len) {
    if (!process_word_symbol(line.substr(start, end - start), symbolized_word)) {
      return false;
    }

    if ((start = end + 1) >= len) {
      break;
    }
    end = next_end();
  }

  if (symbolized_word.symbols.empty()) {
    return false;
  }

  // store the symbolized version of the word in the look-up map;
  symbolized_word.id = ++m_word_count;
  m_word_symbols[word].push_back(symbolized_word);

  // store a look-up from the symbolized word (id) back to the word itself;
  assert(m_word_ids.find(symbolized_word.id) == std::end(m_word_ids));
  m_word_ids[symbolized_word.id] = word;

  auto symbol_count = static_cast<int>(symbolized_word.symbols.size());
  return insert_node(symbolized_word.id, m_nodes, symbol_count - 1, symbolized_word.symbols);
}

bool Rhyme_dictionary::process_word_symbol(const std::string& symbol, Rhyme_dictionary::Symbolized_word& symbolized_word) {
  auto symbol_id = m_symbol_map->get_symbol_id(symbol);
  if (symbol_id == -1) {
    return false;
  }
  symbolized_word.symbols.push_back(symbol_id);
  return true;
}

bool Rhyme_dictionary::insert_node(Word_id word_id, Rhyme_dictionary::Symbol_group& group, int index, const Rhyme_dictionary::Symbols& symbols) {
  auto symbol_id = symbols[index];
  // find or create the matching symbol node;
  auto iter = group.find(symbol_id);
  if (iter == std::end(group)) {
    auto result = group.insert(std::make_pair(symbol_id, std::unique_ptr<Symbol_node>(new Symbol_node())));
    if (!result.second) {
      return false;
    }
    iter = result.first;
  }

  auto symbol_node = iter->second.get();
  // if this is the "last" symbol of a word, store the word here;
  if (index == 0) {
    symbol_node->word_ids.push_back(word_id);
  // otherwise, continue inserting symbols;
  } else {
    if (!insert_node(word_id, symbol_node->children, index - 1, symbols)) {
      return false;
    }
  }
  return true;
}

void Rhyme_dictionary::get_nodes(Symbol_group const& group, int curr, int end, const Symbols& symbols, Word_results& output, bool match_full) const {
  auto current = group.find(symbols[curr]);
  if (current == std::end(group)) {
    return;
  }
  assert(current->second);
  auto const& node = current->second;
  // if we are at the stopping point for rhyme detection;
  if (curr == end) {
    // if the ending node itself has words, gather them directly;
    for (auto const& word_id : node->word_ids) {
      output.insert(m_word_ids.find(word_id)->second);
    }
    // continue to match words based on the stopping criteria;
    get_matching_words(node->children, output, match_full);
    return;
  }
  // otherwise, attempt to continue matching ending symbols;
  get_nodes(node->children, curr - 1, end, symbols, output, match_full);
}

void Rhyme_dictionary::get_matching_words(Symbol_group const& group, Word_results& output, bool match_full) const {
  // if match_full is true, all words below this point are gathered; otherwise it stops at the next non-consonant;
  for (auto const& node : group) {
    // gather any words at this node;
    if (match_full || m_symbol_map->is_consonant(node.first)) {
      for (auto const& word_id : node.second->word_ids) {
        output.insert(m_word_ids.find(word_id)->second);
      }
    // continue gathering words from child nodes;
      get_matching_words(node.second->children, output, match_full);
    }
  }
}

void Rhyme_dictionary::clear() {
  m_word_count = 0;
  m_word_symbols.clear();
  m_word_ids.clear();
  m_symbol_map->clear();
  m_nodes.clear();
}
