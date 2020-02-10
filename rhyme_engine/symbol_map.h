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

#pragma once

using Symbol_id = int;

class Symbol_map {
public:

  Symbol_map();

  Symbol_map(Symbol_map const&) = delete;

  Symbol_map& operator=(Symbol_map const&) = delete;

  ~Symbol_map();

  bool load_symbols(const std::string& symbol_file);

  void clear();

  Symbol_id get_symbol_id(const std::string& symbol) const;

  bool is_consonant(Symbol_id symbol_id) const;

private:

  using Symbol_ids = std::unordered_map<std::string, Symbol_id>;
  using Symbol_vowels = std::unordered_map<Symbol_id, bool>;

  Symbol_ids m_symbol_ids;
  Symbol_vowels m_symbol_vowels;
};
