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
#include <unordered_map>
#include <assert.h>

#include "symbol_map.h"

Symbol_map::Symbol_map() {
}

Symbol_map::~Symbol_map() {
}

bool Symbol_map::load_symbols(const std::string& filename) {
  clear();

  auto input = std::ifstream(filename);
  if (!input) {
    return false;
  }

  static auto const vowels = std::string{"AEIOU"};

  auto id = Symbol_id{0};
  auto symbol = std::string{};
  while (std::getline(input, symbol)) {
    // load each symbol type, one per line;
    if (symbol.empty() || (m_symbol_ids.find(symbol) != std::end(m_symbol_ids))) {
      continue;
    }
    m_symbol_ids[symbol] = id;
    m_symbol_vowels[id] = (vowels.find_first_of(symbol.at(0)) != std::string::npos);
    id++;
  }
  return true;
}

void Symbol_map::clear() {
  m_symbol_ids.clear();
  m_symbol_vowels.clear();
}

Symbol_id Symbol_map::get_symbol_id(const std::string& symbol) const {
  auto iter = m_symbol_ids.find(symbol);
  return (iter == std::end(m_symbol_ids)) ? -1 : iter->second;
}

bool Symbol_map::is_consonant(Symbol_id symbol_id) const {
  auto iter = m_symbol_vowels.find(symbol_id);
  assert(iter != std::end(m_symbol_vowels));
  return (iter == std::end(m_symbol_vowels)) ? false : !iter->second;
}
