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

class Rhyme_dictionary
{
public:

  Rhyme_dictionary();

  Rhyme_dictionary(Rhyme_dictionary const&) = delete;

  Rhyme_dictionary& operator=(Rhyme_dictionary const&) = delete;

  ~Rhyme_dictionary();

  bool load(const std::string& dictionary_file, const std::string& symbol_file);

  void clear();

  bool has_word(const std::string& word) const;

  int get_word_count() const;

  std::vector<std::string> find_rhymes(const std::string& word, bool match_full = false) const;

private:

  using Word_id = int;
  using Word_results = std::set<std::string>;

  struct Symbol_node;
  using  Symbol_group = std::unordered_map<int, std::unique_ptr<Symbol_node>>;
  using  Symbols = std::vector<Symbol_id>;

  struct Symbolized_word {
    Word_id id = 0;
    Symbols symbols;
  };

  struct Symbol_node {
    std::vector<Word_id> word_ids;
    Symbol_group children;
  };

private:

  bool process_word(const std::string& word, const std::string& line);

  bool process_word_symbol(const std::string& symbol, Rhyme_dictionary::Symbolized_word& symbol_word);

  bool insert_node(Word_id word_id, Rhyme_dictionary::Symbol_group& group, int index, const Rhyme_dictionary::Symbols& symbols);

  void get_nodes(Symbol_group const& group, int end_index, int index, const Symbols& symbols, Word_results& output, bool match_full) const;

  void get_matching_words(Symbol_group const& group, Word_results& output, bool match_full) const;

  int m_word_count = 0;

  Symbol_group m_nodes;

  std::unordered_map<std::string, std::vector<Symbolized_word>> m_word_symbols;

  std::unordered_map<Word_id, std::string> m_word_ids;

  std::unique_ptr<Symbol_map> m_symbol_map;
};
